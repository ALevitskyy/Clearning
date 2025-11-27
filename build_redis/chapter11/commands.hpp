#include <map>
#include <string.h>
#include "hashtable.hpp"
#include "serialization.hpp"
#include "utils.hpp"
#include "zset.hpp"

#define container_of(ptr, type, member) ({ \
  const typeof( ((type*)0)->member ) *__mptr = (ptr); \
  (type *)((char *)__mptr - offsetof(type, member)); })

static bool str2dbl(const std::string &s, double &out)
{
  char *endp = NULL;
  out = strtod(s.c_str(), &endp);
  return endp == s.c_str() + s.size() && !isnan(out);
}

static bool str2int(const std::string &s, int64_t &out)
{
  char *endp = NULL;
  out = strtoll(s.c_str(), &endp, 10);
  return endp == s.c_str() + s.size();
}

struct Entry
{
  struct HNode node;
  std::string key;
  std::string val;
};

static struct
{
  HMap db;
} g_data;

static uint64_t str_hash(const uint8_t *data, size_t len)
{
  uint32_t h = 0x811C9DC5;
  for (size_t i = 0; i < len; i++)
  {
    h = (h + data[i]) * 0x01000193;
  }
  return h;
}

static bool entry_eq(HNode *lhs, HNode *rhs)
{
  struct Entry *le = container_of(lhs, struct Entry, node);
  struct Entry *re = container_of(rhs, struct Entry, node);
  return lhs->hcode == rhs->hcode && le->key == re->key;
}

static void do_get(
    std::vector<std::string> &cmd, std::string &out)
{
  Entry key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
  if (!node)
  {
    return out_nil(out);
  }
  const std::string &val = container_of(node, Entry, node)->val;
  return out_str(out, val);
}

static void do_set(
    std::vector<std::string> &cmd, std::string &out)
{

  Entry key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());

  HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
  if (node)
  {
    container_of(node, Entry, node)->val.swap(cmd[2]);
  }
  else
  {
    Entry *ent = new Entry();
    ent->key.swap(key.key);
    ent->node.hcode = key.node.hcode;
    ent->val.swap(cmd[2]);
    hm_insert(&g_data.db, &ent->node);
  }
  return out_nil(out);
}

static void do_del(std::vector<std::string> &cmd, std::string &out)
{
  Entry key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
  if (node)
  {
    delete container_of(node, Entry, node);
  }
  return out_int(out, node ? 1 : 0);
}

static int32_t parse_req(
    const uint8_t *data, size_t len, std::vector<std::string> &out)
{
  if (len < 4)
  {
    return -1;
  }
  uint32_t n = 0;
  memcpy(&n, &data[0], 4);
  const size_t k_max_args = 200 * 1000;
  if (n > k_max_args)
  {
    return -1;
  }
  size_t pos = 4;
  while (n--)
  {
    if (pos + 4 > len)
    {
      return -1;
    }
    uint32_t sz = 0;
    memcpy(&sz, &data[pos], 4);
    if (pos + 4 + sz > len)
    {
      return -1;
    }
    out.push_back(std::string((char *)&data[pos + 4], sz));
    pos += 4 + sz;
  }
  if (pos != len)
  {
    return -1;
  }
  return 0;
}

static void h_scan(HTab *tab, void (*f)(HNode *, void *), void *arg)
{
  if (tab->size == 0)
  {
    return;
  }
  for (size_t i = 0; i < tab->mask + 1; ++i)
  {
    HNode *node = tab->tab[i];
    while (node)
    {
      f(node, arg);
      node = node->next;
    }
  }
}

static void cb_scan(HNode *node, void *arg)
{
  std::string &out = *(std::string *)arg;
  out_str(out, container_of(node, Entry, node)->key);
}

static void do_keys(std::vector<std::string> &cmd, std::string &out)
{
  (void)cmd;
  out_arr(out, (uint32_t)hm_size(&g_data.db));
  h_scan(&g_data.db.ht1, &cb_scan, &out);
  h_scan(&g_data.db.ht2, &cb_scan, &out);
}

static void do_zadd(std::vector<std::string> &cmd, Buffer &out)
{
  double score = 0;
  if (!str2dbl(cmd[2], score))
  {
    return out_err(out, ERR_BAD_ARG, "expect float");
  }

  // look up or create the zset
  LookupKey key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *hnode = hm_lookup(&g_data.db, &key.node, &entry_eq);

  Entry *ent = NULL;
  if (!hnode)
  { // insert a new key
    ent = entry_new(T_ZSET);
    ent->key.swap(key.key);
    ent->node.hcode = key.node.hcode;
    hm_insert(&g_data.db, &ent->node);
  }
  else
  { // check the existing key
    ent = container_of(hnode, Entry, node);
    if (ent->type != T_ZSET)
    {
      return out_err(out, ERR_BAD_TYP, "expect zset");
    }
  }

  // add or update the tuple
  const std::string &name = cmd[3];
  bool added = zset_add(&ent->zset, name.data(), name.size(), score);
  return out_int(out, (int64_t)added);
}

static const ZSet k_empty_zset;

static ZSet *expect_zset(std::string &s)
{
  LookupKey key;
  key.key.swap(s);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *hnode = hm_lookup(&g_data.db, &key.node, &entry_eq);
  if (!hnode)
  { // a non-existent key is treated as an empty zset
    return (ZSet *)&k_empty_zset;
  }
  Entry *ent = container_of(hnode, Entry, node);
  return ent->type == T_ZSET ? &ent->zset : NULL;
}

// zrem zset name
static void do_zrem(std::vector<std::string> &cmd, Buffer &out)
{
  ZSet *zset = expect_zset(cmd[1]);
  if (!zset)
  {
    return out_err(out, ERR_BAD_TYP, "expect zset");
  }

  const std::string &name = cmd[2];
  ZNode *znode = zset_lookup(zset, name.data(), name.size());
  if (znode)
  {
    zset_delete(zset, znode);
  }
  return out_int(out, znode ? 1 : 0);
}

// zscore zset name
static void do_zscore(std::vector<std::string> &cmd, Buffer &out)
{
  ZSet *zset = expect_zset(cmd[1]);
  if (!zset)
  {
    return out_err(out, ERR_BAD_TYP, "expect zset");
  }

  const std::string &name = cmd[2];
  ZNode *znode = zset_lookup(zset, name.data(), name.size());
  return znode ? out_dbl(out, znode->score) : out_nil(out);
}

// zquery zset score name offset limit
static void do_zquery(std::vector<std::string> &cmd, Buffer &out)
{
  // parse args
  double score = 0;
  if (!str2dbl(cmd[2], score))
  {
    return out_err(out, ERR_BAD_ARG, "expect fp number");
  }
  const std::string &name = cmd[3];
  int64_t offset = 0, limit = 0;
  if (!str2int(cmd[4], offset) || !str2int(cmd[5], limit))
  {
    return out_err(out, ERR_BAD_ARG, "expect int");
  }

  // get the zset
  ZSet *zset = expect_zset(cmd[1]);
  if (!zset)
  {
    return out_err(out, ERR_BAD_TYP, "expect zset");
  }

  // seek to the key
  if (limit <= 0)
  {
    return out_arr(out, 0);
  }
  ZNode *znode = zset_seekge(zset, score, name.data(), name.size());
  znode = znode_offset(znode, offset);

  // output
  size_t ctx = out_begin_arr(out);
  int64_t n = 0;
  while (znode && n < limit)
  {
    out_str(out, znode->name, znode->len);
    out_dbl(out, znode->score);
    znode = znode_offset(znode, +1);
    n += 2;
  }
  out_end_arr(out, ctx, (uint32_t)n);
}

static void do_request(std::vector<std::string> &cmd, std::string &out)
{

  if (cmd.size() == 1 && cmd[0] == "keys")
  {
    do_keys(cmd, out);
  }
  else if (cmd.size() == 2 && cmd[0] == "get")
  {
    do_get(cmd, out);
  }
  else if (cmd.size() == 3 && cmd[0] == "set")
  {
    do_set(cmd, out);
  }
  else if (cmd.size() == 2 && cmd[0] == "del")
  {
    do_del(cmd, out);
  }
  else if (cmd.size() == 4 && cmd[0] == "zadd")
  {
    return do_zadd(cmd, out);
  }
  else if (cmd.size() == 3 && cmd[0] == "zrem")
  {
    return do_zrem(cmd, out);
  }
  else if (cmd.size() == 3 && cmd[0] == "zscore")
  {
    return do_zscore(cmd, out);
  }
  else if (cmd.size() == 6 && cmd[0] == "zquery")
  {
    return do_zquery(cmd, out);
  }
  else
  {
    out_err(out, ERR_UNKNOWN, "Unknown cmd");
  }
}
