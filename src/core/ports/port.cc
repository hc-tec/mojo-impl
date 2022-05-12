//
// Created by titto on 2022/5/8.
//

#include "port.h"

namespace tit {
namespace mojo {
namespace ports {

bool Port::IsNextEvent(const NodeName& from_node, const Event& event) {
  return true;
}

void Port::NextEvent(NodeName* from_node, Event::Ptr* event) {

}

void Port::TakePendingMessage(std::vector<Event::Ptr>* messages) {

}

void Port::BufferEvent(const NodeName& from_node, const Event::Ptr& event) {
  auto& event_heap = control_event_queues_[{from_node, event->from_port()}];
  event_heap.push_back(event);
}

}  // namespace ports
}  // namespace mojo
}  // namespace tit