#include <map>
#include <string.h>
#include "hashtable.hpp"
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

enum
{
  RES_OK = 0,
  RES_ERR = 1,
  RES_NX = 2
};

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

static uint32_t do_get(
    std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  Entry key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
  if (!node)
  {
    return RES_NX;
  }
  const std::string &val = container_of(node, Entry, node)->val;
  assert(val.size() <= k_max_msg);
  memcpy(res, val.data(), val.size());
  *reslen = (uint32_t)val.size();
  return RES_OK;
}

static uint32_t do_set(
    std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  (void)res;
  (void)reslen;

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
  return RES_OK;
}

static uint32_t do_del(
    std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  (void)res;
  (void)reslen;
  Entry key;
  key.key.swap(cmd[1]);
  key.node.hcode = str_hash((uint8_t *)key.key.data(), key.key.size());
  HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
  if (node)
  {
    delete container_of(node, Entry, node);
  }
  return RES_OK;
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

static int32_t do_request(
    const uint8_t *req,
    uint32_t reqlen,
    uint32_t *rescode,
    uint8_t *res,
    uint32_t *reslen)
{
  std::vector<std::string> cmd;
  if (0 != parse_req(req, reqlen, cmd))
  {
    msg("bad req");
    return -1;
  }
  if (cmd.size() == 2 && cmd[0] == "get")
  {
    *rescode = do_get(cmd, res, reslen);
  }
  else if (cmd.size() == 3 && cmd[0] == "set")
  {
    *rescode = do_set(cmd, res, reslen);
  }
  else if (cmd.size() == 2 && cmd[0] == "del")
  {
    *rescode = do_del(cmd, res, reslen);
  }
  else
  {
    *rescode = RES_ERR;
    const char *msg = "Unknown cmd";
    strcpy((char *)res, msg);
    *reslen = strlen(msg);
    return 0;
  }
  return 0;
}
