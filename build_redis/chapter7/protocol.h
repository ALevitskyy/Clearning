#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

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

static int32_t read_full(int fd, char *buf, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = read(fd, buf, n);
    if (rv <= 0)
    {
      return -1;
    };
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = write(fd, buf, n);
    if (rv <= 0)
    {
      return -1;
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}
