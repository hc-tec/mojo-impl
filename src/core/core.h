//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_CORE_H
#define MOJO_IMPL_CORE_H

#include "core/io_task_runner.h"
#include "core/handle_table.h"
#include "core/node_controller.h"

namespace tit {
namespace mojo {

class Core {
 public:
  Core();

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;

  ~Core() = default;

  static Core* Get();

  void SetIOTaskRunner(IOTaskRunner* io_task_runner);

  NodeController* GetNodeController();

  Dispatcher::Ptr GetDispatcher(MojoHandle handle);

  MojoHandle AddDispatcher(const Dispatcher::Ptr& dispatcher);

  // MessagePipe API

  MojoHandle CreatePartialMessagePipe(ports::PortRef* peer);

  MojoHandle CreatePartialMessagePipe(const ports::PortRef& peer);

  MojoResult CreateMessagePipe(MojoHandle* message_pip_handle0,
                               MojoHandle* message_pip_handle1);

  MojoResult WriteMessage(MojoHandle message_pipe_producer_handle,
                       const std::string& data);

  MojoResult ReadMessage(MojoHandle message_pipe_consumer_handle,
                      ports::Event::Ptr& event);

  // Invitation API
  MojoResult CreateInvitation(MojoHandle* invitation_handle);
  MojoResult AttachMessagePipeToInvitation(
      MojoHandle invitation_handle,
      const std::string& name,
      MojoHandle* message_pipe_handle);
  MojoResult ExtractMessagePipeFromInvitation(
      MojoHandle invitation_handle,
      const std::string& name,
      MojoHandle* message_pipe_handle);

  MojoResult SendInvitation(
      MojoHandle invitation_handle,
      int socket);
  MojoResult AcceptInvitation(
      MojoHandle* invitation_handle,
      int socket
      );

 private:
  MojoHandle ExtractMessagePipeFromInvitation(
      const std::string& name);
 private:
  HandleTable::Ptr handle_table_;

  base::MutexLock node_controller_lock_;
  NodeController::Ptr node_controller_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_CORE_H
