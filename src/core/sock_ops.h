//
// Created by titto on 2022/5/7.
//

#ifndef MOJO_IMPL_SOCK_OPS_H
#define MOJO_IMPL_SOCK_OPS_H

#include <sys/socket.h>
#include <fcntl.h>

namespace tit {
namespace mojo {
namespace sock {

void set_nonblock(int fd);

void set_cloexec(int fd);

int Socket(int domain, int type, int protocol);

int Recv(int fd, void* buf, int n);

int Recvn(int fd, void* buf, int n);

int Send(int fd, const void* buf, int n);

}  // namespace sock
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_SOCK_OPS_H
