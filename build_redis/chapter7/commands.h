#include <map>

enum
{
  RES_OK = 0,
  RES_ERR = 1,
  RES_NX = 2
};

static std::map<std::string, std::string> g_map;
static uint32_t do_get(
    const std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  if (!g_map.count(cmd[1]))
  {
    return RES_NX;
  }
  std::string &val = g_map[cmd[1]];
  assert(val.size() <= k_max_msg);
  memcpy(res, val.data(), val.size());
  *reslen = (uint32_t)val.size();
  return RES_OK;
}

static uint32_t do_set(
    const std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  (void)res;
  (void)reslen;
  g_map[cmd[1]] = cmd[2];
  return RES_OK;
}

static uint32_t do_del(
    const std::vector<std::string> &cmd,
    uint8_t *res,
    uint32_t *reslen)
{
  (void)res;
  (void)reslen;
  g_map.erase(cmd[1]);
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
