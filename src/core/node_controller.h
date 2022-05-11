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
    return std::make_unique<NodeController>();
  }

  NodeController() {
    GenerateRandomName(&name_);
    node_ = ports::Node::Create(name_, this);
  }

  void SetIOTaskRunner(IOTaskRunner* io_task_runner);

  // NodeChannel::Delegate
  void OnAcceptInvitee(const ports::NodeName& from_node,
                       const ports::NodeName& inviter_name,
                       const ports::NodeName& token) override;
  void OnAcceptInvitation(const ports::NodeName& from_node,
                          const ports::NodeName& token,
                          const ports::NodeName& invitee_name) override;
  void OnRequestPortMerge(const ports::NodeName& from_node,
                          const ports::PortName& connector_port_name,
                          const std::string& message_pipe_name) override;
  void OnResponsePortMerge(const ports::NodeName& from_node,
                          const ports::PortName& connector_port_name,
                           const ports::PortName& port_name) override;
  void OnAddClient(const ports::NodeName& from_node,
                   const ports::NodeName& client_name,
                   int process_handle) override;
  void OnClientAdded(const ports::NodeName& from_node,
                     const ports::NodeName& client_name, int channel) override;
  void OnAcceptClient(const ports::NodeName& from_node,
                      const ports::NodeName& client_name, int channel) override;
  void OnEventMessage(const ports::NodeName& from_node,
                      const Protocol::Ptr& message) override;
  void OnChannelError(const ports::NodeName &name,
                      NodeChannel *channel) override;

  // ports::Node::Delegate
  void ForwardEvent(const ports::NodeName &node,
                    const ports::Event::Ptr &event) override;
  void BroadcastEvent(const ports::Event &event) override;
  void PortStatusChanged(const ports::PortRef &port_ref) override;

  void SendInvitation(
      ConnectionParams connection_params,
      const std::vector<std::pair<std::string, ports::PortRef>>& attached_ports);

  void AcceptInvitation(ConnectionParams connection_params);

  void AddPeer(const ports::NodeName& name,
                           const NodeChannel::Ptr& channel,
                           bool start_channel);
  void DropPeer(const ports::NodeName& name);
  NodeChannel::Ptr GetPeerChannel(const ports::NodeName& name);

  NodeChannel::Ptr GetInviterChannel();

  void MergePortIntoInviter(const std::string& name,
                            const ports::PortRef& port);

  ports::Node* node() { return node_.get(); }
  void OnAcceptPeer(const ports::NodeName& from_node,
                    const ports::NodeName& token,
                    const ports::NodeName& peer_name,
                    const ports::PortName& port_name) override;

 private:
  void HandlePendingMergePort();

 private:
  using NodeMap = std::unordered_map<ports::NodeName,
                                     NodeChannel::Ptr>;

  ports::NodeName name_;

  ports::Node::Ptr node_;
  IOTaskRunner* io_task_runner_;

  base::MutexLock inviter_lock_;
  ports::NodeName inviter_name_;
  NodeChannel::Ptr inviter_channel_;

  base::MutexLock peers_lock_;
  NodeMap peers_ GUARDED_BY(peers_lock_);

  base::MutexLock reserved_ports_lock_;
  std::map<ports::NodeName, PortMap> reserved_ports_
      GUARDED_BY(reserved_ports_lock_);

  // Guards |pending_port_merges_| and |reject_pending_merges_|.
  base::MutexLock pending_port_merges_lock_;

  // A set of port merge requests awaiting inviter connection.
  std::vector<std::pair<std::string, ports::PortRef>> pending_port_merges_
      GUARDED_BY(pending_port_merges_lock_);;

  // Channels to invitees during handshake.
  NodeMap pending_invitations_;

};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_NODE_CONTROLLER_H
