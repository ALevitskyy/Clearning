#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <assert.h>
#include "protocol.h"

static int32_t query(int fd, const char *text)
{
  if (int32_t err = send_message(fd, text))
  {
    return err;
  }
  char rbuf[4 + k_max_msg + 1];
  int32_t len = parse_request(fd, rbuf);
  if (len < 0)
  {
    return len; // Error case;
  }
  rbuf[4 + len] = '\0';
  printf("server says: %s\n", &rbuf[4]);
  return 0;
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  }
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
  int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    die("connect");
  }
  int32_t err = query(fd, "hello1");
  if (err)
  {
    goto L_DONE;
  }
  err = query(fd, "hello2");
  if (err)
  {
    goto L_DONE;
  }
  err = query(fd, "hello3");
  if (err)
  {
    goto L_DONE;
  }

L_DONE:
  close(fd);
  return 0;
}
