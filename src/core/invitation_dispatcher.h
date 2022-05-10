//
// Created by titto on 2022/5/10.
//

#ifndef MOJO_IMPL_INVITATIONDISPATCHER_H
#define MOJO_IMPL_INVITATIONDISPATCHER_H

#include <map>

#include "base/mutex.h"
#include "core/dispatcher.h"
#include "core/ports/port_ref.h"

namespace tit {
namespace mojo {


class InvitationDispatcher : public Dispatcher {
 public:
  using Ptr = std::shared_ptr<InvitationDispatcher>;
  using PortMapping = std::map<std::string, ports::PortRef>;

  static Ptr Create();

  InvitationDispatcher() = default;

  Type GetType() const override;

  MojoResult Close() override;

  MojoResult AttachMessagePipe(const std::string& name,
                               ports::PortRef remote_peer_port);

  MojoResult ExtractMessagePipe(const std::string& name,
                                MojoHandle* message_pipe_handle);

  PortMapping TakeAttachPorts();

  DISALLOW_COPY_AND_ASSIGN(InvitationDispatcher);
 private:
  base::MutexLock lock_;
  PortMapping attached_ports_;

  bool is_closed_ { false };
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_INVITATIONDISPATCHER_H
