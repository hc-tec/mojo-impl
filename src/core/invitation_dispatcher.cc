//
// Created by titto on 2022/5/10.
//

#include "invitation_dispatcher.h"

#include "core/core.h"

namespace tit {
namespace mojo {

Dispatcher::Type InvitationDispatcher::GetType() const {
  return Type::kInvitation;
}

MojoResult InvitationDispatcher::AttachMessagePipe(
    const std::string& name, ports::PortRef remote_peer_port) {
  base::MutexLockGuard g(lock_);
  auto r = attached_ports_.emplace(name, remote_peer_port);
  if (!r.second) {
    return MOJO_RESULT_ALREADY_EXISTS;
  }
  return MOJO_RESULT_OK;
}

InvitationDispatcher::PortMapping InvitationDispatcher::TakeAttachPorts() {
  PortMapping attached_ports;
  {
    base::MutexLockGuard g(lock_);
    std::swap(attached_ports, attached_ports_);
  }
  return attached_ports;
}

InvitationDispatcher::Ptr InvitationDispatcher::Create() {
  return std::make_shared<InvitationDispatcher>();
}

MojoResult InvitationDispatcher::ExtractMessagePipe(
    const std::string& name, MojoHandle* message_pipe_handle) {

  ports::PortRef remote_peer_port;
  {
    base::MutexLockGuard g(lock_);
    auto it = attached_ports_.find(name);
    if (it == attached_ports_.end())
      return MOJO_RESULT_NOT_FOUND;
    remote_peer_port = std::move(it->second);
    attached_ports_.erase(it);
  }

  *message_pipe_handle = Core::Get()->CreatePartialMessagePipe(
      remote_peer_port);
  if (*message_pipe_handle == MOJO_HANDLE_INVALID)
    return MOJO_RESULT_RESOURCE_EXHAUSTED;

  return MOJO_RESULT_OK;
}

}  // namespace mojo
}  // namespace tit