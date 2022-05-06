//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_IO_TASK_RUNNER_H
#define MOJO_IMPL_IO_TASK_RUNNER_H

#include "core/def.h"

namespace tit {
namespace mojo {

enum IOEvent : uint8 {
  kReadable = 1 << 0,
  kWritable = 1 << 1,
  kReadWritable = kReadable | kWritable,
};

// A interface for channel posix to operate its socket
class IOTaskRunner {
 public:

  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnFdReadable(int fd) = 0;
    virtual void OnFdWriteable(int fd) = 0;
    virtual void OnFdError(int fd) = 0;
  };

  ~IOTaskRunner() {
    delegate_ = nullptr;
  }

  Delegate* delegate() const { return delegate_; }

  void set_delegate(Delegate* delegate) {
    delegate_ = delegate;
  }

  virtual void AddFdEvent(int fd, IOEvent event) = 0;
  virtual void DelFdEvent(int fd, IOEvent event) = 0;

 private:
  Delegate* delegate_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_IO_TASK_RUNNER_H
