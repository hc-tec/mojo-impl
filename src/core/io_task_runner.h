//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_IO_TASK_RUNNER_H
#define MOJO_IMPL_IO_TASK_RUNNER_H

#include <map>

#include "base/def.h"

namespace tit {
namespace mojo {

enum IOEvent : uint8 {
  kReadable = 1 << 1,
  kWritable = 1 << 2,
  kReadWritable = kReadable | kWritable,
};

// A interface for channel posix to operate its socket
class IOTaskRunner {
 public:

  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual int handle() = 0;
    virtual void OnFdReadable(int fd) = 0;
    virtual void OnFdWriteable(int fd) = 0;
//    virtual void OnFdError(int fd) = 0;
  };

  ~IOTaskRunner() {

  }

  Delegate* delegate(int fd) const {
    auto pair = delegate_map_.find(fd);
    if (pair == delegate_map_.end()) return nullptr;
    return pair->second;
  }

  void set_delegate(Delegate* delegate) {
    delegate_map_[delegate->handle()] = delegate;
  }

  virtual void AddFdEvent(int fd, IOEvent event) = 0;
  virtual void DelFdEvent(int fd, IOEvent event) = 0;

 private:
  std::map<int, Delegate*> delegate_map_ {};
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_IO_TASK_RUNNER_H
