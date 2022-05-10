//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_CHANNEL_POSIX_H
#define MOJO_IMPL_CHANNEL_POSIX_H

#include "core/channel.h"

#include <sys/socket.h>
#include <queue>
#include <deque>

#include "base/mutex.h"
#include "core/connection_params.h"
#include "core/io_task_runner.h"

namespace tit {
namespace mojo {

static constexpr int kInvalidSocket = -1;

class ChannelPosix : public Channel,
                     public IOTaskRunner::Delegate {
 public:

  ChannelPosix(Channel::Delegate* delegate,
               ConnectionParams connection_params,
               IOTaskRunner* io_task_runner)
      : Channel(delegate,
                connection_params,
                io_task_runner),
        socket_(connection_params.fd()) {
    io_task_runner_->set_delegate(this);
  }

  ~ChannelPosix() override = default;

  int handle() override { return socket_; }

  // Channel
  void Start() override;
  void Read() override;
  void Write(const Protocol::Ptr& protocol) override;
  void ShutdownImpl() override;

  // IOTaskRunner::Delegate
  void OnFdReadable(int fd) override;
  void OnFdWriteable(int fd) override;

 private:

  std::string ReadFixBufFromSocket(uint16 fix_len) const;

  Protocol::Ptr ReadHeader(int header_length) const;

  std::string ReadBody(int content_length);

 private:
  int socket_;
//  base::MutexLock lock_;
  std::deque<int> incoming_fds_;
  std::queue<Protocol::Ptr> writing_protocol_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_CHANNEL_POSIX_H
