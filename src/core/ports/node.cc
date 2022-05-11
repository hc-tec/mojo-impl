//
// Created by titto on 2022/5/8.
//

#include "node.h"

#include "base/mutex.h"
#include "base/noncopyable.h"
#include "core/ports/event.h"
#include "core/protocols/response_port_merge.h"

namespace tit {
namespace mojo {
namespace ports {

constexpr size_t kRandomNameCacheSize = 256;

class RandomNameGenerator {
 public:
  RandomNameGenerator() = default;
  ~RandomNameGenerator() = default;

  PortName GenerateRandomPortName() {
    base::MutexLockGuard lock(lock_);
    if (cache_index_ == kRandomNameCacheSize) {
      base::RandBytes(cache_, sizeof(PortName) * kRandomNameCacheSize);
      cache_index_ = 0;
    }
    return cache_[cache_index_++];
  }

  DISALLOW_COPY_AND_ASSIGN(RandomNameGenerator);
 private:
  base::MutexLock lock_;
  PortName cache_[kRandomNameCacheSize];
  size_t cache_index_ = kRandomNameCacheSize;
};

RandomNameGenerator g_name_generator;

void GenerateRandomPortName(PortName* name) {
  *name = g_name_generator.GenerateRandomPortName();
}

Node::DelegateHolder::DelegateHolder(
    Node *node,
    Node::Delegate *delegate)
    : node_(node),
      delegate_(delegate) {}

Node::DelegateHolder::~DelegateHolder() {
  node_ = nullptr;
  delegate_ = nullptr;
}

Node::Node(const NodeName &name, Node::Delegate *delegate)
    : name_(name),
      delegate_(delegate) {}

Node::~Node() {
  delegate_ = nullptr;
}

int Node::SendUserMessageInternal(const PortRef& port_ref,
                                  const Event::Ptr& message) {
//  UserMessageEvent::Ptr msg = UserMessageEvent::Ptr();
  message->set_from_port(port_ref.name());
  message->set_port_name(port_ref.port()->peer_port_name);
  delegate_->ForwardEvent(port_ref.port()->peer_node_name, message);
}

int Node::CreateUninitializedPort(PortRef* port_ref) {
  PortName port_name;
  GenerateRandomPortName(&port_name);
  Port::Ptr port = Port::Create();
  int r = AddPortWithName(port_name, port);
  if (r != OK)
    return r;
  *port_ref = PortRef(port_name, port);
  return OK;
}

int Node::AddPortWithName(const PortName& port_name,
                          const Port::Ptr& port) {
  base::MutexLockGuard g(ports_lock_);
  if (port->peer_port_name != kInvalidPortName) {
    peer_port_maps_[port->peer_node_name][port->peer_port_name].emplace(
      port_name, PortRef(port_name, port)
    );
  }
  if (!ports_.emplace(port_name, port).second) {
    return ERROR_PORT_EXISTS;
  }
  LOG(DEBUG) << "Create port name @" << port_name;
  return OK;
}

int Node::InitializePort(const PortRef& port_ref,
                         const NodeName& peer_node_name,
                         const PortName& peer_port_name) {
  base::MutexLockGuard g(ports_lock_);
  auto port = port_ref.port();
  if (port->state != Port::State::kUninitialized) {
    return ERROR_PORT_STATE_UNEXPECTED;
  }
  port->state = Port::State::kReceiving;
  UpdatePortPeerAddress(port_ref.name(),
                        port,
                        peer_node_name,
                        peer_port_name);
  return OK;
}

void Node::UpdatePortPeerAddress(const PortName& local_port_name,
                                 const Port::Ptr& local_port,
                                 const NodeName& new_peer_node,
                                 const PortName& new_peer_port) {
  local_port->peer_node_name = new_peer_node;
  local_port->peer_port_name = new_peer_port;
  if (new_peer_port != ports::kInvalidPortName) {
    peer_port_maps_[new_peer_node][new_peer_port].emplace(
        local_port_name,
        PortRef(local_port_name, local_port));
  }
}

int Node::CreatePortPair(PortRef* port0_ref, PortRef* port1_ref) {
  int r = CreateUninitializedPort(port0_ref);
  if (r != OK)
    return r;
  r = CreateUninitializedPort(port1_ref);
  if (r != OK)
    return r;
  r = InitializePort(*port0_ref, name_, port1_ref->name());
  if (r != OK)
    return r;
  r = InitializePort(*port1_ref, name_, port0_ref->name());
  if (r != OK)
    return r;
  return OK;
}

int Node::ClosePort(const PortRef& port_ref) {
  std::vector<Event::Ptr> messages;
  auto port = port_ref.port();
  port->TakePendingMessage(&messages);
  ErasePort(port_ref.name());
}

void Node::ErasePort(const PortName& port_name) {
  ports::Port::Ptr port;
  {
    base::MutexLockGuard g(ports_lock_);
    auto it = ports_.find(port_name);
    if (it == ports_.end())
      return;
    port = std::move(it->second);
    ports_.erase(it);
  }
  LOG(TRACE) << "Deleted port " << port_name << "@" << name_;
}

int Node::MergePorts(
    PortName* local_port_name,
    const PortRef& port_ref,
    const NodeName& destination_node_name,
    const PortName& destination_port_name) {

  ErasePort(port_ref.name());

  Port::Ptr remote_port = port_ref.port();
  Port::Ptr local_port;
  {
    base::MutexLockGuard g(ports_lock_);
    *local_port_name = remote_port->peer_port_name;
    auto it = ports_.find(*local_port_name);
    if (it == ports_.end()) {
      LOG(ERROR) << "Ignored";
      return ERROR_PORT_UNKNOWN;
    }
    local_port = it->second;
  }
  UpdatePortPeerAddress(*local_port_name, local_port,
                        destination_node_name, destination_port_name);
  return OK;
}

int Node::MergePortsResponse(const PortName& local_peer_port_name,
                             const NodeName& destination_node_name,
                             const PortName& destination_port_name) {
  Port::Ptr local_peer_port;
  {
    base::MutexLockGuard g(ports_lock_);
    auto it = ports_.find(local_peer_port_name);
    if (it == ports_.end()) {
      LOG(ERROR) << "Ignored";
      return ERROR_PORT_UNKNOWN;
    }
    local_peer_port = it->second;
  }
  PortName local_port_name = local_peer_port->peer_port_name;
  PortRef local_port = peer_port_maps_[name_]
                                      [local_peer_port_name]
                                      [local_port_name];
  PortName dummy;
  MergePorts(&dummy, local_port, destination_node_name, destination_port_name);
  return 0;
}

}  // namespace ports

}  // namespace mojo
}  // namespace tit
