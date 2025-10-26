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

static int32_t read_res(int fd)
{
  // Returns -1 if error, otherwise 0
  char rbuf[4 + k_max_msg + 1];
  errno = 0;
  int32_t err = read_full(fd, rbuf, 4);
  if (err)
  {
    log_err();
    return err;
  }
  uint32_t len = 0;
  memcpy(&len, rbuf, 4); // Cool trick
  if (len > k_max_msg)
  {
    msg("too long");
    return -1;
  }
  err = read_full(fd, &rbuf[4], len);
  if (err)
  {
    msg("read() error");
    return err;
  }

  rbuf[4 + len] = '\0';
  printf("server says: %s\n", &rbuf[4]);
  return 0;
}

static int32_t send_message(int fd, const char *text)
{
  char wbuf[4 + sizeof(text)];
  int32_t len = (uint32_t)strlen(text);
  if (len > k_max_msg)
  {
    msg("Sending too long text");
    return -1;
  }
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], text, len);
  return write_all(fd, wbuf, 4 + len);
}
