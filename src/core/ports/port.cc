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

void Port::BufferEvent(const NodeName& from_node, Event::Ptr event) {

}

void Port::TakePendingMessage(std::vector<UserMessageEvent::Ptr>* messages) {

}

}  // namespace ports
}  // namespace mojo
}  // namespace tit