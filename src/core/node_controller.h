//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_NODE_CONTROLLER_H
#define MOJO_IMPL_NODE_CONTROLLER_H

#include <memory>
#include <unordered_map>

#include "core/io_task_runner.h"
#include "core/node_channel.h"
#include "core/ports/node.h"

namespace tit {
namespace mojo {

class NodeController : public NodeChannel::Delegate,
                       public ports::Node::Delegate {
 public:
  using Ptr = std::unique_ptr<NodeController>;
  using PortMap = std::map<std::string, ports::PortRef>;
  static Ptr Create() {
    return std::unique_ptr<NodeController>();
  }

  void SetIOTaskRunner(IOTaskRunner* io_task_runner);

  // NodeChannel::Delegate
  void OnAcceptInvitee(const ports::NodeName& from_node,
                       const ports::NodeName& inviter_name,
                       const ports::NodeName& token) override;
  void OnAcceptInvitation(const ports::NodeName& from_node,
                          const ports::NodeName& token,
                          const ports::NodeName& invitee_name) override;
  void OnChannelError(const ports::NodeName &name,
                      NodeChannel *channel) override;

  // ports::Node::Delegate
  void ForwardEvent(const ports::NodeName &node,
                    const ports::Event &event) override;
  void BroadcastEvent(const ports::Event &event) override;
  void PortStatusChanged(const ports::PortRef &port_ref) override;


  void AddPeer(const ports::NodeName& name,
                           const NodeChannel::Ptr& channel,
                           bool start_channel);
  void DropPeer(const ports::NodeName& name);
  NodeChannel::Ptr GetPeerChannel(const ports::NodeName& name);

  NodeChannel::Ptr GetInviterChannel();

  void MergePortIntoInviter(const std::string& name,
                            const ports::PortRef& port);

  ports::Node* node() { return node_.get(); }

 private:
  using NodeMap = std::unordered_map<ports::NodeName,
                                     NodeChannel::Ptr>;

  ports::NodeName name_;

  ports::Node::Ptr node_;
  IOTaskRunner* io_task_runner_;

  base::MutexLock inviter_lock_;
  ports::NodeName inviter_name_;

  base::MutexLock peers_lock_;
  NodeMap peers_ GUARDED_BY(peers_lock_);

  base::MutexLock reserved_ports_lock_;
  std::map<ports::NodeName, PortMap> reserved_ports_
      GUARDED_BY(reserved_ports_lock_);

  // Channels to invitees during handshake.
  NodeMap pending_invitations_;

};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_NODE_CONTROLLER_H
