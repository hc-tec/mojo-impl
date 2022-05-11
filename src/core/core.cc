//
// Created by titto on 2022/5/8.
//

#include "core.h"

#include "core/invitation_dispatcher.h"
#include "core/message_pipe_dispatcher.h"

namespace tit {
namespace mojo {

Core::Core() {
  handle_table_ = HandleTable::Create();
  node_controller_ = NodeController::Create();
}

Core* Core::Get() {
  static Core g_core;
  return &g_core;
}

void Core::SetIOTaskRunner(IOTaskRunner* io_task_runner) {
  node_controller_->SetIOTaskRunner(io_task_runner);
}

NodeController* Core::GetNodeController() {
  base::MutexLockGuard g(node_controller_lock_);
  return node_controller_.get();
}

Dispatcher::Ptr Core::GetDispatcher(MojoHandle handle) {
  return handle_table_->GetDispatcher(handle);
}

MojoHandle Core::AddDispatcher(const Dispatcher::Ptr& dispatcher) {
  return handle_table_->AddDispatcher(dispatcher);
}

MojoResult Core::CreateInvitation(MojoHandle* invitation_handle) {

  *invitation_handle = AddDispatcher(
      InvitationDispatcher::Create());

  if (*invitation_handle == MOJO_HANDLE_INVALID)
    return MOJO_RESULT_RESOURCE_EXHAUSTED;

  return MOJO_RESULT_OK;
}

MojoHandle Core::CreatePartialMessagePipe(ports::PortRef* peer) {
  ports::PortRef local_port;
  GetNodeController()->node()->CreatePortPair(&local_port, peer);
  return AddDispatcher(
      MessagePipeDispatcher::Create(
          GetNodeController(),
          local_port)
      );
}

MojoHandle Core::CreatePartialMessagePipe(const ports::PortRef& port) {
  return AddDispatcher(
      MessagePipeDispatcher::Create(
          GetNodeController(),
          port)
  );
}

MojoResult Core::AttachMessagePipeToInvitation(
    MojoHandle invitation_handle,
    const std::string& name,
    MojoHandle* message_pipe_handle) {
  Dispatcher::Ptr dispatcher = GetDispatcher(invitation_handle);
  if (!dispatcher || dispatcher->GetType() != Dispatcher::Type::kInvitation)
    return MOJO_RESULT_INVALID_ARGUMENT;
  auto* invitation_dispatcher = static_cast<InvitationDispatcher*>(
      dispatcher.get()
      );
  ports::PortRef remote_peer_port;
  MojoHandle local_handle = CreatePartialMessagePipe(&remote_peer_port);
  if (local_handle == MOJO_HANDLE_INVALID)
    return MOJO_RESULT_RESOURCE_EXHAUSTED;

  MojoResult r = invitation_dispatcher->AttachMessagePipe(
      name, remote_peer_port);
  if (r != MOJO_RESULT_OK)
    return r;
  *message_pipe_handle = local_handle;
  return MOJO_RESULT_OK;
}

MojoResult Core::ExtractMessagePipeFromInvitation(
    MojoHandle invitation_handle, const std::string& name,
    MojoHandle* message_pipe_handle) {
  Dispatcher::Ptr dispatcher = GetDispatcher(invitation_handle);
  if (!dispatcher || dispatcher->GetType() != Dispatcher::Type::kInvitation)
    return MOJO_RESULT_INVALID_ARGUMENT;
  auto* invitation_dispatcher = static_cast<InvitationDispatcher*>(
      dispatcher.get()
  );

  MojoResult r = invitation_dispatcher->ExtractMessagePipe(name, message_pipe_handle);

  if (r != MOJO_RESULT_OK)
    return r;

  *message_pipe_handle = ExtractMessagePipeFromInvitation(name);

  if (*message_pipe_handle == MOJO_HANDLE_INVALID)
    return MOJO_RESULT_RESOURCE_EXHAUSTED;
  return MOJO_RESULT_OK;
}

MojoHandle Core::ExtractMessagePipeFromInvitation(const std::string& name) {
  ports::PortRef port0, port1;
  GetNodeController()->node()->CreatePortPair(&port0, &port1);
  MojoHandle handle = AddDispatcher(MessagePipeDispatcher::Create(
      GetNodeController(), port0));
  GetNodeController()->MergePortIntoInviter(name, port1);
  return handle;
}

MojoResult Core::SendInvitation(MojoHandle invitation_handle,
                                int socket) {

  Dispatcher::Ptr dispatcher = GetDispatcher(invitation_handle);
  if (!dispatcher || dispatcher->GetType() != Dispatcher::Type::kInvitation)
    return MOJO_RESULT_INVALID_ARGUMENT;
  auto* invitation_dispatcher =
      static_cast<InvitationDispatcher*>(dispatcher.get());

  ConnectionParams connection_params(socket);

  // TODO: remove dispatcher, take the ownership of dispatcher

  std::vector<std::pair<std::string, ports::PortRef>> attached_ports;
  InvitationDispatcher::PortMapping attached_port_map =
      invitation_dispatcher->TakeAttachPorts();
//  invitation_dispatcher->Close();
  for (auto& entry : attached_port_map)
    attached_ports.emplace_back(entry.first, std::move(entry.second));

  invitation_dispatcher->TakeAttachPorts();
  GetNodeController()->SendInvitation(connection_params, attached_ports);

  return MOJO_RESULT_OK;
}

MojoResult Core::AcceptInvitation(MojoHandle* invitation_handle,
                                  int socket) {

  auto dispatcher = InvitationDispatcher::Create();
  *invitation_handle = AddDispatcher(dispatcher);
  if (*invitation_handle == MOJO_HANDLE_INVALID)
    return MOJO_RESULT_RESOURCE_EXHAUSTED;
  ConnectionParams params(socket);
  GetNodeController()->AcceptInvitation(params);
  return MOJO_RESULT_OK;
}

}  // namespace mojo
}  // namespace tit