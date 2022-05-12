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
#include "core/protocols/response_port_merge.h"

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
    case MsgType::kAcceptInvitation: {
      LOG(TRACE) << "Msg: Accept Invitation";
      AcceptInvitationProtocol::Ptr data = AcceptInvitationProtocol::Create();
      Channel::DeserializeMessage(data, protocol->content());
      delegate_->OnAcceptInvitation(remote_node_name_,
                                    data->token_,
                                    data->invitee_name_);
      return;
    }
    case MsgType::kAcceptInvitee: {
      LOG(TRACE) << "Msg: Accept Invitee";
      AcceptInviteeProtocol::Ptr data = AcceptInviteeProtocol::Create();
      Channel::DeserializeMessage(data, protocol->content());
      delegate_->OnAcceptInvitee(remote_node_name_,
                                 data->inviter_name_,
                                 data->token_);
      return;
    }
    case MsgType::kRequestPortMerge: {
      LOG(TRACE) << "Msg: Request Port Merge";
      RequestPortMergeProtocol::Ptr data = RequestPortMergeProtocol::Create();
      Channel::DeserializeMessage(data, protocol->content());
      delegate_->OnRequestPortMerge(remote_node_name_,
                                    data->connector_port_name_,
                                    data->message_pipe_name_);
      return;
    }
    case MsgType::kResponsePortMerge: {
      LOG(TRACE) << "Msg: Response Port Merge";
      ResponsePortMergeProtocol::Ptr data = ResponsePortMergeProtocol::Create();
      Channel::DeserializeMessage(data, protocol->content());
      delegate_->OnResponsePortMerge(remote_node_name_,
                                    data->connector_port_name_,
                                    data->port_name_);
      return;
    }
    case MsgType::kUserMessage: {
      LOG(TRACE) << "Msg: User Message";
      UserMessage::Ptr data = UserMessage::Create();
      Channel::DeserializeMessage(data, protocol->content());
      delegate_->OnUserMessage(remote_node_name_, data);
      return;
    }
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
                                   const std::string &message_pipe_name) {
  LOG(TRACE) << "Ready send RequestPortMerge data";
  RequestPortMergeProtocol::Ptr data = RequestPortMergeProtocol::Create();
  data->message_pipe_name_ = message_pipe_name;
  data->connector_port_name_ = connector_port_name;
  Protocol::Ptr message = Protocol::Create(
      data->type(), Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::AcceptInvitee(const ports::NodeName &inviter_name,
                                const ports::NodeName &token) {
  LOG(TRACE) << "Ready send AcceptInvitee data";
  AcceptInviteeProtocol::Ptr data = AcceptInviteeProtocol::Create();
  data->token_ = token;
  data->inviter_name_ = inviter_name;
  Protocol::Ptr message = Protocol::Create(
      data->type(), Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::AcceptInvitation(const ports::NodeName &token,
                                   const ports::NodeName &invitee_name) {
  LOG(TRACE) << "Ready send AcceptInvitation data";
  AcceptInvitationProtocol::Ptr data = AcceptInvitationProtocol::Create();
  data->token_ = token;
  data->invitee_name_ = invitee_name;
  Protocol::Ptr message = Protocol::Create(
      data->type(), Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::AcceptPeer(const ports::NodeName &sender_name,
                             const ports::NodeName &token,
                             const ports::PortName &port_name) {
  LOG(TRACE) << "Ready send AcceptPeer data";
  AcceptPeerProtocol::Ptr data = AcceptPeerProtocol::Create();
  data->token_ = token;
  data->port_name_ = port_name;
  data->peer_name_ = sender_name;
  Protocol::Ptr message = Protocol::Create(
      data->type(), Channel::SerializeMessage(data));
  SendChannelMessage(message);
}

void NodeChannel::ResponsePortMerge(const ports::PortName &connector_port_name,
                                    const ports::PortName &port_name) {
  LOG(TRACE) << "Ready send ResponsePortMerge data";
  ResponsePortMergeProtocol::Ptr data = ResponsePortMergeProtocol::Create();
  data->connector_port_name_ = connector_port_name;
  data->port_name_ = port_name;
  Protocol::Ptr message = Protocol::Create(
      data->type(), Channel::SerializeMessage(data));
  SendChannelMessage(message);

}

}  // namespace mojo

}  // namespace tit