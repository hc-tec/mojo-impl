#pragma once

#include "core/libuv_io_task_runner_adapter.h"
#include "core/channel_posix.h"
#include "core/sock_ops.h"
#include "core/protocol.h"
#include "core/ports/user_message.h"

using namespace tit;

int main() {
  int socket_pair[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);
  mojo::sock::set_nonblock(socket_pair[0]);
  mojo::sock::set_nonblock(socket_pair[1]);

  mojo::LibuvIOTaskRunnerAdapter* io_task_runner =
      new mojo::LibuvIOTaskRunnerAdapter();

  mojo::ChannelPosix* write_channel =
      new mojo::ChannelPosix(nullptr, io_task_runner, socket_pair[0]);
  mojo::ChannelPosix* read_channel =
      new mojo::ChannelPosix(nullptr, io_task_runner, socket_pair[1]);
  write_channel->Start();
  read_channel->Start();
  mojo::ports::PortName port_name(123, 456);
  std::string data("hello world");
  mojo::ports::UserMessage::Ptr message =
      mojo::ports::UserMessage::Create(port_name, data);
  mojo::Protocol::Ptr protocol =
      mojo::Protocol::Create(
          mojo::Protocol::MsgType::kNormal,
          message->Encode()->toString());
  write_channel->Write(protocol);

  io_task_runner->Run();
  return 0;
}
