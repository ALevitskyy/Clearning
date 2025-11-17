#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <string>
#include <string.h>
#include <stdint.h>

enum
{
  SER_NIL = 0,
  SER_ERR = 1,
  SER_STR = 2,
  SER_INT = 3,
  SER_ARR = 4
};

static void out_nil(std::string &out)
{
  out.push_back(SER_NIL);
}

static void out_str(std::string &out, const std::string &val)
{
  out.push_back(SER_STR);
  uint32_t len = (uint32_t)val.size();
  out.append((char *)&len, 4);
  out.append(val);
}

static void out_int(std::string &out, int64_t val)
{
  out.push_back(SER_INT);
  out.append((char *)&val, 8);
}

static void out_err(std::string &out, int32_t code, const std::string &msg)
{
  out.push_back(SER_ERR);
  out.append((char *)&code, 4);
  uint32_t len = (uint32_t)msg.size();
  out.append((char *)&len, 4);
  out.append(msg);
}

static void out_arr(std::string &out, uint32_t n)
{
  out.push_back(SER_ARR);
  out.append((char *)&n, 4);
}

static int32_t on_response(const uint8_t *data, size_t size)
{
  if (size < 1)
  {
    msg("bad response");
    return -1;
  }
  switch (data[0])
  {
  case SER_NIL:
    printf("(nil)\n");
    return 1;
  case SER_ERR:
    if (size < 1 + 8)
    {
      msg("bad response");
      return -1;
    }
    {
      int32_t code = 0;
      uint32_t len = 9;
      memcpy(&code, &data[1], 4);
      memcpy(&len, &data[1 + 4], 4);
      if (size < 1 + 8 + len)
      {
        msg("bad response");
        return -1;
      }
      printf("(err) %d %.*s\n", code, len, &data[1 + 8]);
      return 1 + 8 + len;
    }
  case SER_STR:
    if (size < 1 + 4)
    {
      msg("bad response");
      return -1;
    }
    {
      uint32_t len = 0;
      memcpy(&len, &data[1], 4);
      if (size < 1 + 4 + len)
      {
        msg("bad response");
        return -1;
      }
      printf("(str) %.*s\n", len, &data[1 + 4]);
      return 1 + 4 + len;
    }
  case SER_INT:
    if (size < 1 + 8)
    {
      msg("bad response");
      return -1;
    }
    {
      int64_t val = 0;
      memcpy(&val, &data[1], 8);
      printf("(int) %ld\n", val);
      return 1 + 8;
    }
  case SER_ARR:
    if (size < 1 + 4)
    {
      msg("bad response");
      return -1;
    }
    {
      uint32_t len = 0;
      memcpy(&len, &data[1], 4);
      printf("(arr) len=%u\n", len);
      size_t arr_bytes = 1 + 4;
      for (uint32_t i = 0; i < len; ++i)
      {
        int32_t rv = on_response(&data[arr_bytes], size - arr_bytes);
        if (rv < 0)
        {
          return rv;
        }
        arr_bytes += (size_t)rv;
      }
      printf("(arr) end\n");
      return (int32_t)arr_bytes;
    }

  default:
    msg("bad response");
    return -1;
  }
}

#endif // SERIALIZATION_HPP
