//
// Created by titto on 2022/5/8.
//

#include "node_controller.h"

#include "log/logging.h"

namespace tit {
namespace mojo {

void NodeController::ForwardEvent(const ports::NodeName& node,
                                  const ports::Event& event) {

  ports::UserMessageEvent::Ptr message_event =
      (const ports::UserMessageEvent::Ptr&)event;

  if (node == name_) {

  } else {

  }


}

void NodeController::SetIOTaskRunner(IOTaskRunner* io_task_runner) {
  io_task_runner_ = io_task_runner;
}

void NodeController::MergePortIntoInviter(const std::string& name,
                                          const ports::PortRef& port) {
  NodeChannel::Ptr inviter = GetInviterChannel();

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

}  // namespace mojo
}  // namespace tit