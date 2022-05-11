//
// Created by titto on 2022/5/9.
//

#ifndef MOJO_IMPL_MESSAGE_PIPE_DISPATCHER_H
#define MOJO_IMPL_MESSAGE_PIPE_DISPATCHER_H

#include <atomic>
#include "core/dispatcher.h"
#include "core/node_controller.h"

namespace tit {
namespace mojo {

class MessagePipeDispatcher : public Dispatcher {
 public:
  using Ptr = std::shared_ptr<MessagePipeDispatcher>;

  static Ptr Create(NodeController* node_controller,
                    const ports::PortRef& port);

  MessagePipeDispatcher(NodeController* node_controller,
                        const ports::PortRef& port);

  // Dispatcher
  Type GetType() const override;
  MojoResult Close() override;

  MojoResult WriteMessage(const ports::Event::Ptr& message);
  MojoResult ReadMessage(ports::Event::Ptr& message);

 private:
  NodeController* node_controller_;
  ports::PortRef port_;
  std::atomic<bool> port_closed_;
};


}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_MESSAGE_PIPE_DISPATCHER_H
