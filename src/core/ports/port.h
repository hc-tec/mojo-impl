//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_PORT_H
#define MOJO_IMPL_PORT_H

#include <map>
#include <memory>
#include <queue>

#include "base/def.h"
#include "base/mutex.h"
#include "core/ports/event.h"
#include "core/ports/message_queue.h"
#include "core/ports/name.h"

namespace tit {
namespace mojo {
namespace ports {

class Port {
 public:
  using Ptr = std::shared_ptr<Port>;

  static Ptr Create() {
    return std::make_shared<Port>();
  }

  enum class State : uint8 {
    kUninitialized,
    kReceiving,
    kBuffering,
    kProxying,
    kClosed
  };

  State state;
  PortName peer_port_name;
  NodeName peer_node_name;

  MessageQueue message_queue;
  using NodeEventPair = std::pair<NodeName, Event::Ptr>;
  std::queue<NodeEventPair> control_message_queue;

  bool peer_closed;
  bool peer_lost_unexpectedly;

  Port() = default;

  bool IsNextEvent(const NodeName& from_node, const Event& event);

  void NextEvent(NodeName* from_node, Event::Ptr* event);

  void BufferEvent(const NodeName& from_node, Event::Ptr event);

  void TakePendingMessage(std::vector<Event::Ptr>* messages);

  DISALLOW_COPY_AND_ASSIGN(Port);

  ~Port() = default;
 private:
  using NodePortPair = std::pair<NodeName, PortName>;
  using EventQueue = std::vector<Event::Ptr>;
  std::map<NodePortPair, EventQueue> control_event_queues_;

  base::MutexLock lock_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_PORT_H
