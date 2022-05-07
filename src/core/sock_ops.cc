//
// Created by titto on 2022/5/7.
//

#include "sock_ops.h"

#include <cerrno>

namespace tit {
namespace mojo {
namespace sock {

void set_nonblock(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void set_cloexec(int fd) {
  fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
}

int Socket(int domain, int type, int protocol) {
  return socket(domain, type | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
}

int Recv(int fd, void* buf, int n) {

  do {
    int r = recv(fd, buf, n, 0);
    if (r != -1) return r;
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return -1;
    } else if (errno != EINTR) {
      return -1;
    }
  } while (true);
}

int Recvn(int fd, void* buf, int n) {
  char* s = (char*) buf;
  int remain = n;

  do {
    int r = recv(fd, s, remain, 0);
    if (r == remain) return n;
    if (r == 0) return 0;

    if (r == -1) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        return -1;
      } else if (errno != EINTR) {
        return -1;
      }
    } else {
      remain -= r;
      s += r;
    }
  } while (true);
}

int Send(int fd, const void* buf, int n) {
  const char* s = (const char*) buf;
  int remain = n;

  do {
    int r = send(fd, s, remain, 0);
    if (r == remain) return n;
    if (r == -1) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        return -1;
      } else if (errno != EINTR) {
        return -1;
      }
    } else {
      remain -= r;
      s += r;
    }
  } while (true);
}

}  // namespace sock
}  // namespace mojo
}  // namespace tit