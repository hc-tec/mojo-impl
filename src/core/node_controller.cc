//
// Created by titto on 2022/5/8.
//

#include "node_controller.h"

#include "core/ports/node.h"
#include "log/logging.h"

namespace tit {
namespace mojo {

void NodeController::ForwardEvent(const ports::NodeName& node,
                                  const ports::Event::Ptr & event) {

//  ports::UserMessageEvent::Ptr message_event =
//      (const ports::UserMessageEvent::Ptr&)event;

  if (node == name_) {

  } else {

  }


}

void NodeController::SetIOTaskRunner(IOTaskRunner* io_task_runner) {
  io_task_runner_ = io_task_runner;
}

void NodeController::MergePortIntoInviter(const std::string& name,
                                          const ports::PortRef& port) {
  NodeChannel::Ptr inviter;
  {
    base::MutexLockGuard g(pending_port_merges_lock_);
    inviter = GetInviterChannel();
    if (!inviter) {
      pending_port_merges_.emplace_back(name, port);
      return;
    }
  }
  inviter->RequestPortMerge(port.name(), name);
}

void NodeController::AddPeer(const ports::NodeName& name,
                             const NodeChannel::Ptr& channel,
                             bool start_channel) {
  channel->SetRemoteNodeName(name);
  {
    base::MutexLockGuard g(peers_lock_);
    if (peers_.find(name) != peers_.end()) {
      LOG(ERROR) << "Ignoring duplicate peer name " << name;
      return;
    }
    auto r = peers_.insert(std::make_pair(name, channel));
    if (!r.second) {
      LOG(ERROR) << "Peers insert error";
      return;
    }
    LOG(DEBUG) << "Accepting new peer " << name << " on node " << name_;
  }
  if (start_channel)
    channel->Start();
}

void NodeController::DropPeer(const ports::NodeName& name) {
  {
    base::MutexLockGuard g(peers_lock_);
    auto it = peers_.find(name);
    if (it != peers_.end()) {
      peers_.erase(it);
      LOG(DEBUG) << "Dropped peer " << name;
    }
  }
  std::vector<ports::PortRef> ports_to_close;
  {
    base::MutexLockGuard g(reserved_ports_lock_);
    auto it = reserved_ports_.find(name);
    if (it != reserved_ports_.end()) {
      for (auto& entry : it->second)
        ports_to_close.emplace_back(entry.second);
      reserved_ports_.erase(it);
    }
  }

  for (const auto& port : ports_to_close)
    node_->ClosePort(port);

}

NodeChannel::Ptr NodeController::GetPeerChannel(const ports::NodeName& name) {
  base::MutexLockGuard g(peers_lock_);
  auto it = peers_.find(name);
  if (it == peers_.end())
    return nullptr;
  return it->second;
}

NodeChannel::Ptr NodeController::GetInviterChannel() {
  ports::NodeName inviter_name;
  {
    base::MutexLockGuard g(inviter_lock_);
    inviter_name = inviter_name_;
  }
  return GetPeerChannel(inviter_name);
}

void NodeController::SendInvitation(
    ConnectionParams connection_params,
    const std::vector<std::pair<std::string, ports::PortRef>>& attached_ports) {
  ports::NodeName temp_node_name;
  GenerateRandomName(&temp_node_name);
  {
    base::MutexLockGuard g(reserved_ports_lock_);
    PortMap& port_map = reserved_ports_[temp_node_name];
    for (auto& entry : attached_ports) {
      auto result = port_map.emplace(entry.first, entry.second);
//      if (result.second) {
//        LOG(ERROR) << "Duplicate attachment: " << entry.first;
//      }
    }
  }

  NodeChannel::Ptr channel = NodeChannel::Create(
      this, connection_params, io_task_runner_);

  pending_invitations_.emplace(temp_node_name, channel);

  channel->SetRemoteNodeName(temp_node_name);
  channel->Start();
  channel->AcceptInvitee(name_, temp_node_name);
}

void NodeController::AcceptInvitation(ConnectionParams connection_params) {

  {
    base::MutexLockGuard g(inviter_lock_);
    inviter_name_ = ports::kInvalidNodeName;
    inviter_channel_ = NodeChannel::Create(
        this, connection_params,
        io_task_runner_);
  }
  inviter_channel_->Start();
}

void NodeController::OnRequestPortMerge(
    const ports::NodeName& from_node,
    const ports::PortName& connector_port_name,
    const std::string& message_pipe_name) {

  ports::PortRef local_port;
  {
    base::MutexLockGuard g(reserved_ports_lock_);
    auto it = reserved_ports_.find(from_node);
    if (it == reserved_ports_.end()) {
      LOG(ERROR) << "Ignoring port merge request from node " << from_node << ". "
                 << "No ports reserved for that node.";
      return;
    }
    PortMap& port_map = it->second;
    auto port_it = port_map.find(message_pipe_name);
    if (port_it == port_map.end()) {
      LOG(ERROR) << "Ignoring request to connect to port for unknown name "
                 << message_pipe_name << " from node " << from_node;
      return;
    }
    local_port = port_it->second;
    port_map.erase(port_it);
    if (port_map.empty())
      reserved_ports_.erase(it);
  }
  ports::PortName local_port_name;
  node_->MergePorts(&local_port_name, local_port,
                    from_node, connector_port_name);
  GetPeerChannel(from_node)->ResponsePortMerge(connector_port_name,
                                               local_port_name);
}

void NodeController::OnAddClient(const ports::NodeName& from_node,
                                 const ports::NodeName& client_name,
                                 int process_handle) {}
void NodeController::OnClientAdded(const ports::NodeName& from_node,
                                   const ports::NodeName& client_name,
                                   int channel) {}
void NodeController::OnAcceptClient(const ports::NodeName& from_node,
                                    const ports::NodeName& client_name,
                                    int channel) {}
void NodeController::OnEventMessage(const ports::NodeName& from_node,
                                    const Protocol::Ptr& message) {}

void NodeController::OnAcceptInvitee(const ports::NodeName& from_node,
                                     const ports::NodeName& inviter_name,
                                     const ports::NodeName& token) {

  {
    base::MutexLockGuard g(inviter_lock_);
    inviter_name_ = inviter_name;
  }

  inviter_channel_->SetRemoteNodeName(inviter_name);
  inviter_channel_->AcceptInvitation(token, name_);
  AddPeer(inviter_name, inviter_channel_,
          false /* start_channel */);
  HandlePendingMergePort();
}

void NodeController::OnAcceptInvitation(const ports::NodeName& from_node,
                                        const ports::NodeName& token,
                                        const ports::NodeName& invitee_name) {
  auto it = pending_invitations_.find(from_node);
  if (it == pending_invitations_.end() || token != from_node) {
    LOG(ERROR) << "Received unexpected AcceptInvitation message from "
               << from_node;
    DropPeer(from_node);
    return;
  }

  {
    base::MutexLockGuard g(reserved_ports_lock_);
    auto reserved_ports_it = reserved_ports_.find(from_node);
    if (reserved_ports_it != reserved_ports_.end()) {
      // Swap the temporary node name's reserved ports into an entry keyed by
      // the real node name.
      auto result = reserved_ports_.emplace(
          invitee_name, std::move(reserved_ports_it->second));
      if (!result.second) {
        LOG(ERROR) << "insert error" << reserved_ports_it->first;
      }
      reserved_ports_.erase(reserved_ports_it);
    }
  }
  NodeChannel::Ptr channel = it->second;
  pending_invitations_.erase(it);
  AddPeer(invitee_name, channel, false);

}

void NodeController::OnChannelError(const ports::NodeName& name,
                                    NodeChannel* channel) {
  LOG(ERROR) << "Error from node name: " << name;
}
void NodeController::OnAcceptPeer(const ports::NodeName& from_node,
                                  const ports::NodeName& token,
                                  const ports::NodeName& peer_name,
                                  const ports::PortName& port_name) {}
void NodeController::BroadcastEvent(const ports::Event& event) {}
void NodeController::PortStatusChanged(const ports::PortRef& port_ref) {}

void NodeController::OnResponsePortMerge(
    const ports::NodeName& from_node,
    const ports::PortName& connector_port_name,
    const ports::PortName& port_name) {
  node_->MergePortsResponse(connector_port_name,from_node, port_name);
}

void NodeController::HandlePendingMergePort() {

  // Complete any port merge requests we have waiting for the inviter.
  std::vector<std::pair<std::string, ports::PortRef>> pending_port_merges;
  {
    base::MutexLockGuard g(pending_port_merges_lock_);
    std::swap(pending_port_merges_, pending_port_merges);
  }
  std::vector<ports::PortName> pending_port_names;
  for (auto& request : pending_port_merges) {
    LOG(TRACE) << "Handle request port merge. message pipe name: "
               << request.first;
    GetInviterChannel()->RequestPortMerge(request.second.name(), request.first);
  }

}

}  // namespace mojo
}  // namespace tit