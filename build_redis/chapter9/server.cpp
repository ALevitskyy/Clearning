#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utils.hpp"
#include "commands.hpp"
#include "connection.hpp"
#include <vector>
#include <fcntl.h>
#include <poll.h>

static bool
try_flush_buffer(Conn *conn)
{
  ssize_t rv = 0;
  do
  {
    size_t remain = conn->wbuf_size - conn->wbuf_sent;
    rv = write(conn->fd, &conn->wbuf[conn->wbuf_sent], remain);
  } while (rv < 0 && errno == EINTR);

  if (rv < 0 && errno == EAGAIN)
  {
    return false;
  }
  if (rv < 0)
  {
    msg("write() error");
    conn->state = STATE_END;
    return false;
  }
  conn->wbuf_sent += (size_t)rv;
  assert(conn->wbuf_sent <= conn->wbuf_size);
  if (conn->wbuf_sent == conn->wbuf_size)
  {
    conn->state = STATE_REQ;
    conn->wbuf_sent = 0;
    conn->wbuf_size = 0;
    return false;
  }
  return true;
}

static void state_res(Conn *conn)
{
  while (try_flush_buffer(conn))
  {
  };
}

static bool try_one_request(Conn *conn)
{
  if (conn->rbuf_size < 4)
  {
    return false;
  }
  uint32_t len = 0;
  memcpy(&len, &conn->rbuf[0], 4);

  std::vector<std::string> cmd;
  if (0 != parse_req(&conn->rbuf[4], len, cmd))
  {
    msg("bad req");
    conn->state = STATE_END;
    return false;
  }

  std::string out;
  do_request(cmd, out);

  if (4 + out.size() > k_max_msg)
  {
    out.clear();
    out_err(out, ERR_TOO_BIG, "resonse is too big");
  }

  uint32_t wlen = (uint32_t)out.size();
  memcpy(&conn->wbuf[0], &wlen, 4);
  memcpy(&conn->wbuf[4], out.data(), out.size());
  conn->wbuf_size = 4 + wlen;

  size_t remain = conn->rbuf_size - 4 - len;
  if (remain)
  {
    memmove(conn->rbuf, &conn->rbuf[4 + len], remain);
  }
  conn->rbuf_size = remain;
  conn->state = STATE_RES;
  state_res(conn);

  return (conn->state = STATE_REQ);
}

static bool try_fill_buffer(Conn *conn)
{
  assert(conn->rbuf_size < sizeof(conn->rbuf));
  ssize_t rv = 0;
  do
  {
    size_t cap = sizeof(conn->rbuf) - conn->rbuf_size;
    rv = read(conn->fd, &conn->rbuf[conn->rbuf_size], cap);
  } while (rv < 0 && errno == EINTR);

  if (rv < 0 && errno == EAGAIN)
  {
    return false;
  }
  if (rv < 0)
  {
    msg("read() error");
    conn->state = STATE_END;
    return false;
  }
  if (rv == 0)
  {
    if (conn->rbuf_size > 0)
    {
      msg("unexpected EOF");
    }
    else
    {
      msg("EOF");
    }
    conn->state = STATE_END;
    return false;
  }
  conn->rbuf_size += (size_t)rv;
  assert(conn->rbuf_size <= sizeof(conn->rbuf) - conn->rbuf_size);
  while (try_one_request(conn))
  {
  }
  return (conn->state == STATE_REQ);
}

static void state_req(Conn *conn)
{
  while (try_fill_buffer(conn))
  {
  }
}

static void connection_io(Conn *conn)
{
  if (conn->state == STATE_REQ)
  {
    state_req(conn);
  }
  else if (conn->state == STATE_RES)
  {
    state_res(conn);
  }
  else
  {
    assert(0);
  }
}

int main()
{
  int fd = init_fd();

  std::vector<Conn *> fd2conn;

  while (true)
  {
    std::vector<struct pollfd> poll_args = poll_connections(fd, fd2conn);
    for (size_t i = 1; i < poll_args.size(); ++i)
    {
      if (poll_args[i].revents)
      {
        Conn *conn = fd2conn[poll_args[i].fd];
        connection_io(conn);
        if (conn->state == STATE_END)
        {
          fd2conn[conn->fd] = NULL;
          (void)close(conn->fd);
          free(conn);
        }
      }
    }
    if (poll_args[0].revents)
    {
      (void)accept_new_conn(fd2conn, fd);
    }
  }
  return 0;
}
