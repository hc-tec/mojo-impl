//
// Created by titto on 2022/5/11.
//

#ifndef MOJO_IMPL_SOCK_PAIR_CHANNEL_H
#define MOJO_IMPL_SOCK_PAIR_CHANNEL_H

#include "core/sock_ops.h"

namespace tit {
namespace mojo {

class SockPairChannel {
 public:
  SockPairChannel() {
    int socket_pair[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);
    local_endpoint_ = socket_pair[0];
    remote_endpoint_ = socket_pair[1];
    sock::set_nonblock(local_endpoint_);
    sock::set_nonblock(remote_endpoint_);
  }

  int local_endpoint() { return local_endpoint_; }
  int remote_endpoint() { return remote_endpoint_; }

 private:
  int local_endpoint_;
  int remote_endpoint_;
};

}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_SOCK_PAIR_CHANNEL_H
