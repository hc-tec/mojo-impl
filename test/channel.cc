#pragma once

#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "base/process.h"
#include "core/channel_posix.h"
#include "core/connection_params.h"
#include "core/libuv_io_task_runner_adapter.h"
#include "core/protocol.h"
#include "core/protocols/user_message.h"
#include "core/sock_ops.h"
#include "core/node_channel.h"
#include "log/logging.h"

using namespace tit;

void AddParentPrefix(
    log::LogStream& stream,
    const log::LogMessageInfo& l,
    void* data) {
  stream << "[parent] " << l.level_
         << "20220406-18:30:30"
         << ' ' << PlatformThread::CurrentId()
         << ' ' << l.fullname_ << ':' << l.line_ << ' ';
}

void AddChildPrefix(
    log::LogStream& stream,
    const log::LogMessageInfo& l,
    void* data) {
  stream << "[Child ] " << l.level_
         << "20220406-18:30:30"
         << ' ' << PlatformThread::CurrentId()
         << ' ' << l.fullname_ << ':' << l.line_ << ' ';
}

int main(int argc, char* argv[]) {
  base::Init(argc, argv);
  log::InitTitLogging(argv[0], AddParentPrefix, nullptr);
  std::string cmd_line = base::CurrentCommandLine();
  base::ArgValueParser<int> int_parser;

  int handler = int_parser("mojo-platform-channel-handle");
  if (handler != (int) INTMAX_MAX) {
    log::InitTitLogging(argv[0], AddChildPrefix, nullptr);
    LOG(INFO) << "Born from parent";
    mojo::LibuvIOTaskRunnerAdapter* io_task_runner =
        new mojo::LibuvIOTaskRunnerAdapter();
    mojo::ConnectionParams params(handler);
    mojo::NodeChannel::Ptr write_channel = mojo::NodeChannel::Create(
        nullptr,
        params,
        io_task_runner);
    write_channel->Start();
    mojo::ports::PortName port_name(123, 456);
    std::string data("hello world");
    mojo::UserMessage::Ptr message =
        mojo::UserMessage::Create(port_name, data);
    mojo::Protocol::Ptr protocol =
        mojo::Protocol::Create(
            mojo::MsgType::kEventMessage,
            message->Encode()->toString());
    write_channel->WriteChannelMessage(protocol);
    io_task_runner->Run();
    return 0;
  }

  int socket_pair[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair);
  mojo::sock::set_nonblock(socket_pair[0]);
  mojo::sock::set_nonblock(socket_pair[1]);

  mojo::LibuvIOTaskRunnerAdapter* io_task_runner =
      new mojo::LibuvIOTaskRunnerAdapter();

  mojo::ConnectionParams params(socket_pair[1]);

  mojo::NodeChannel::Ptr node_channel = mojo::NodeChannel::Create(
      nullptr,
      params,
      io_task_runner);

  node_channel->Start();

  std::string argument("-mojo-platform-channel-handle=");
  argument.append(std::to_string(socket_pair[0]));
  char* argument_list[] = {
      const_cast<char*>(base::CurrentExecuteName().data()),
      const_cast<char*>(argument.data()), NULL};

  Process* child_process = base::Process::LaunchProcess(argument_list);
  delete child_process;

  mojo::ports::PortName port_name(123, 456);
  std::string data("send from reader");
  mojo::UserMessage::Ptr message =
      mojo::UserMessage::Create(port_name, data);
  mojo::Protocol::Ptr protocol =
      mojo::Protocol::Create(
          mojo::MsgType::kEventMessage,
          message->Encode()->toString());
  node_channel->WriteChannelMessage(protocol);

  io_task_runner->Run();

  return 0;
}
