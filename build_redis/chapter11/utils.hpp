#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

enum
{
  ERR_UNKNOWN = 1, // unknown command
  ERR_TOO_BIG = 2, // response too big
};

inline void die(const char *msg)
{
  int err = errno;
  fprintf(stderr, "[%d] %s\n", err, msg);
  abort();
}

inline void msg(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
}

inline void log_err()
{
  if (errno == 0)
  {
    msg("server EOF");
  }
  else
  {
    msg("read() error");
  }
}

const int32_t k_max_msg = 4096;

#endif // UTILS_H
