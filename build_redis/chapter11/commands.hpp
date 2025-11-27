#include <map>
#include <string.h>
#include "hashtable.hpp"
#include "serialization.hpp"
#include "utils.hpp"

#define container_of(ptr, type, member) ({ \
  const typeof( ((type*)0)->member ) *__mptr = (ptr); \
  (type *)((char *)__mptr - offsetof(type, member)); })

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
  else
  {
    out_err(out, ERR_UNKNOWN, "Unknown cmd");
  }
}
