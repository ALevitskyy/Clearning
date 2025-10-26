#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static void die(const char *msg)
{
  int err = errno;
  fprintf(stderr, "[%d] %s\n", err, msg);
  abort();
}

static void msg(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
}

static void log_err()
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
