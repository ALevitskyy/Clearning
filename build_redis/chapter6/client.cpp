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
  const char *query_list[3] = {"hello1", "hello2", "hello3"};
  for (size_t i = 0; i < 3; ++i)
  {
    int32_t err = send_message(fd, query_list[i]);
    if (err)
    {
      goto L_DONE;
    }
  }
  for (size_t i = 0; i < 3; ++i)
  {
    int32_t err = read_res(fd);
    if (err)
    {
      goto L_DONE;
    }
  }

L_DONE:
  close(fd);
  return 0;
}
