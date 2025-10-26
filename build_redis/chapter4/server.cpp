#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "protocol.h"

static int32_t one_request(int connfd)
{
  char rbuf[4 + k_max_msg + 1];
  int32_t len = parse_request(connfd, rbuf);
  if (len < 0)
  {
    return len; // Error case;
  }

  rbuf[4 + len] = '\0';
  printf("client says: %s\n", &rbuf[4]);

  const char reply[] = "world";
  return send_message(connfd, reply);
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  };
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(0);
  int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    die("bind()");
  }
  rv = listen(fd, SOMAXCONN);
  if (rv)
  {
    die("listen()");
  }
  while (true)
  {
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0)
    {
      continue;
    }
    while (true)
    {
      int32_t err = one_request(connfd);
      if (err)
      {
        break;
      }
    };
    close(connfd);
  }
}
