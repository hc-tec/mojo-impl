//
// Created by titto on 2022/5/6.
//

#include "core/channel_posix.h"

namespace tit {
namespace mojo {

void ChannelPosix::Start() {
  io_task_runner_->AddFdEvent(socket_, IOEvent::kReadWritable);
}

void ChannelPosix::Read() {}

void ChannelPosix::Write(const Protocol::Ptr& protocol) {}

void ChannelPosix::ShutdownImpl() {
  io_task_runner_ = nullptr;
}

void ChannelPosix::OnFdReadable(int fd) {  }
void ChannelPosix::OnFdWriteable(int fd) {  }

}  // namespace mojo
}  // namespace tit