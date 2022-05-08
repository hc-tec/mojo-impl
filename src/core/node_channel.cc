//
// Created by titto on 2022/5/8.
//

#include "node_channel.h"

#include "core/ports/user_message.h"
#include "log/logging.h"

namespace tit {
namespace mojo {

NodeChannel::NodeChannel(NodeChannel::Delegate *delegate,
                         ConnectionParams connection_params,
                         IOTaskRunner *io_task_runner)
    : delegate_(delegate),
      channel_(Channel::Create(this,
                               connection_params,
                               io_task_runner)) {}

void NodeChannel::Start() {
  base::MutexLockGuard g(channel_lock_);
  if (channel_) {
    channel_->Start();
  }
}

void NodeChannel::Shutdown() {
  base::MutexLockGuard g(channel_lock_);
  if (channel_) {
    channel_->Shutdown();
    channel_ = nullptr;
  }
}

void NodeChannel::SendChannelMessage(const Protocol::Ptr &message) {
  WriteChannelMessage(message);
}

void NodeChannel::WriteChannelMessage(const Protocol::Ptr &message) {
  base::MutexLockGuard g(channel_lock_);
  if (!channel_) {
    LOG(ERROR) << "";
    return;
  }
  channel_->Write(message);
}


void NodeChannel::OnChannelMessage(const Protocol::Ptr& protocol) {
  ports::UserMessage* message =
      dynamic_cast<ports::UserMessage *>(protocol->next_layer().get());

  switch (message->msg_type()) {

    case ports::MsgType::kEventMessage:
      return;

    case ports::MsgType::kAcceptInvitee:
      return;
    case ports::MsgType::kAcceptInvitation:
      return;
    case ports::MsgType::kAcceptPeer:
      return;
  }

  LOG(ERROR) << "Received invalid message type: "
              << static_cast<int>(message->msg_type()) << " closing channel.";
  delegate_->OnChannelError(remote_node_name_, this);
}

void NodeChannel::OnChannelError() {
  Shutdown();
  delegate_->OnChannelError(remote_node_name_, this);
}

}  // namespace mojo
}  // namespace tit