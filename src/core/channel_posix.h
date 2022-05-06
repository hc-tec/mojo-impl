//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_CHANNEL_POSIX_H
#define MOJO_IMPL_CHANNEL_POSIX_H

#include <sys/socket.h>

#include <deque>

#include "base/mutex.h"
#include "core/channel.h"
#include "core/io_task_runner.h"

namespace tit {
namespace mojo {

static constexpr int kInvalidSocket = -1;

class ChannelPosix : public Channel,
                     public IOTaskRunner::Delegate {
 public:

  ChannelPosix(Channel::Delegate* delegate,
               IOTaskRunner* io_task_runner)
      : Channel(delegate),
        socket_(kInvalidSocket),
        io_task_runner_(io_task_runner) {
    io_task_runner_->set_delegate(this);
  }

  ~ChannelPosix() override = default;

  // Channel
  void Start() override;
  void Read() override;
  void Write(const Protocol::Ptr& protocol) override;
  void ShutdownImpl() override;

  // IOTaskRunner::Delegate
  void OnFdReadable(int fd) override;
  void OnFdWriteable(int fd) override;

 private:
  int socket_;
  IOTaskRunner* io_task_runner_;
//  base::MutexLock lock_;
  std::deque<int> incoming_fds_;

};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_CHANNEL_POSIX_H
