#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include <vector>       // for std::vector
#include <sys/socket.h> // for accept(), sockaddr_in, socklen_t
#include <netinet/in.h> // for sockaddr_in
#include <unistd.h>     // for close()
#include <fcntl.h>      // for fcntl(), O_NONBLOCK
#include <errno.h>      // for errno
#include <stdlib.h>
#include "utils.h"
#include <poll.h>

enum
{
  STATE_REQ = 0, // reading requests
  STATE_RES = 1, // sending responses
  STATE_END = 2,
};

struct Conn
{
  int fd = -1;
  // buffer for reading
  uint32_t state = 0;
  size_t rbuf_size = 0;
  uint8_t rbuf[4 + k_max_msg];
  // buffer for writing
  size_t wbuf_size = 0;
  size_t wbuf_sent = 0;
  uint8_t wbuf[4 + k_max_msg];
};

static void fd_set_nb(int fd)
{
  errno = 0;
  int flags = fcntl(fd, F_GETFL, 0);
  if (errno)
  {
    die("fcntl error");
    return;
  }

  flags |= O_NONBLOCK;

  errno = 0;
  (void)fcntl(fd, F_SETFL, flags);
  if (errno)
  {
    die("fcntl error");
  }
}

static void conn_put(std::vector<Conn *> &fd2conn, struct Conn *conn)
{
  if (fd2conn.size() <= (size_t)conn->fd)
  {
    fd2conn.resize(conn->fd + 1);
  }
  fd2conn[conn->fd] = conn;
}

static int32_t accept_new_conn(std::vector<Conn *> &fd2conn, int fd)
{
  struct sockaddr_in client_addr = {};
  socklen_t socklen = sizeof(client_addr);
  int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
  if (connfd < 0)
  {
    msg("accept() error");
    return -1;
  }
  fd_set_nb(connfd);
  struct Conn *conn = (struct Conn *)malloc(sizeof(struct Conn));
  if (!conn)
  {
    close(connfd);
    return -1;
  }
  conn->fd = connfd;
  conn->state = STATE_REQ;
  conn->rbuf_size = 0;
  conn->wbuf_size = 0;
  conn->wbuf_sent = 0;
  conn_put(fd2conn, conn);
  return 0;
}

static int init_fd()
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
  fd_set_nb(fd);
  return fd;
}

static std::vector<struct pollfd> get_poll_args(int fd, std::vector<Conn *> &fd2conn)
{
  std::vector<struct pollfd> poll_args;
  poll_args.clear();
  struct pollfd pfd = {fd, POLLIN, 0};
  poll_args.push_back(pfd);
  for (Conn *conn : fd2conn)
  {
    if (!conn)
    {
      continue;
    }
    struct pollfd pfd = {};
    pfd.fd = conn->fd;
    pfd.events = (conn->state == STATE_REQ) ? POLLIN : POLLOUT;
    pfd.events = pfd.events | POLLERR;
    poll_args.push_back(pfd);
  }
  return poll_args;
}

static std::vector<struct pollfd> poll_connections(int fd, std::vector<Conn *> &fd2conn)
{
  std::vector<struct pollfd> poll_args = get_poll_args(fd, fd2conn);
  int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), 1000);
  if (rv < 0)
  {
    die("poll");
  }
  return poll_args;
}

#endif // CONNECTION_H
