//
// Created by titto on 2022/5/9.
//

#include "message_pipe_dispatcher.h"

namespace tit {
namespace mojo {

MessagePipeDispatcher::MessagePipeDispatcher(NodeController *node_controller,
                                             const ports::PortRef &port)
    : node_controller_(node_controller),
      port_(port) {}

Dispatcher::Type MessagePipeDispatcher::GetType() const {
  return Type::kMessagePipe;
}

MessagePipeDispatcher::Ptr MessagePipeDispatcher::Create(
    NodeController *node_controller, const ports::PortRef &port) {
  return std::make_shared<MessagePipeDispatcher>(
      node_controller, port);
}

}  // namespace mojo
}  // namespace tit