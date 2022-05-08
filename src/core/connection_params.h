//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_CONNECTION_PARAMS_H
#define MOJO_IMPL_CONNECTION_PARAMS_H

namespace tit {
namespace mojo {

class ConnectionParams {
 public:

  ConnectionParams(int fd)
      : fd_(fd) {}

  int fd() const { return fd_; }

  void set_is_async(bool is_async) { is_async_ = is_async; }
  bool is_async() const { return is_async_; }

 private:
  bool is_async_ { false };
  int fd_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_CONNECTION_PARAMS_H
