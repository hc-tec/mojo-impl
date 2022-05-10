//
// Created by titto on 2022/5/8.
//

#include "node_channel.h"

#include "core/serializer.h"
#include "core/protocols/accept_peer.h"
#include "core/protocols/accept_invitee.h"
#include "core/protocols/accept_invitation.h"
#include "core/protocols/user_message.h"
#include "core/protocols/request_port_marge.h"

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
  Protocol::Ptr message;

  switch (protocol->msg_type()) {
    case MsgType::kAcceptInvitation:
      message = Protocol::TransToProtocolPtr<AcceptInvitationProtocol>();
      Channel::DeserializeMessage(message, protocol->content());
    case MsgType::kRequestPortMerge:
      message = Protocol::TransToProtocolPtr<RequestPortMergeProtocol>();
      Channel::DeserializeMessage(message, protocol->content());
      return;
    case MsgType::kEventMessage:
      message = Protocol::TransToProtocolPtr<UserMessage>();
      Channel::DeserializeMessage(message, protocol->content());
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

void NodeChannel::SetRemoteNodeName(const ports::NodeName &name) {
  remote_node_name_ = name;
}

void NodeChannel::RequestPortMerge(const ports::PortName &connector_port_name,
                                   const std::string &token) {



}

void NodeChannel::AcceptInvitee(const ports::NodeName &inviter_name,
                                const ports::NodeName &token) {
  AcceptInviteeProtocol::Ptr data = AcceptInviteeProtocol::Create();
  data->token_ = token;
  data->inviter_name_ = inviter_name;
  Protocol::Ptr message = Protocol::Create(
      MsgType::kAcceptInvitee,Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::AcceptInvitation(const ports::NodeName &token,
                                   const ports::NodeName &invitee_name) {
  AcceptInvitationProtocol::Ptr data = AcceptInvitationProtocol::Create();
  data->token_ = token;
  data->invitee_name_ = invitee_name;
  Protocol::Ptr message = Protocol::Create(
      MsgType::kAcceptInvitation,Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::AcceptPeer(const ports::NodeName &sender_name,
                             const ports::NodeName &token,
                             const ports::PortName &port_name) {
  AcceptPeerProtocol::Ptr data = AcceptPeerProtocol::Create();
  data->token_ = token;
  data->port_name_ = port_name;
  data->peer_name_ = sender_name;
  Protocol::Ptr message = Protocol::Create(
      MsgType::kAcceptPeer,Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

}  // namespace mojo

}  // namespace tit