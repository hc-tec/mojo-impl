//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_NODE_CHANNEL_H
#define MOJO_IMPL_NODE_CHANNEL_H

#include <memory>

#include "base/mutex.h"
#include "core/channel.h"
#include "core/ports/name.h"
#include "core/io_task_runner.h"

namespace tit {
namespace mojo {


class NodeChannel : public Channel::Delegate {
 public:
  using Ptr = std::shared_ptr<NodeChannel>;

  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnAcceptInvitee(const ports::NodeName& from_node,
                                 const ports::NodeName& inviter_name,
                                 const ports::NodeName& token) = 0;
    virtual void OnAcceptInvitation(const ports::NodeName& from_node,
                                    const ports::NodeName& token,
                                    const ports::NodeName& invitee_name) = 0;
    virtual void OnRequestPortMerge(const ports::NodeName& from_node,
                                    const ports::PortName& connector_port_name,
                                    const std::string& token) = 0;
    virtual void OnAddClient(const ports::NodeName& from_node,
                                   const ports::NodeName& client_name,
                                   int process_handle) = 0;
    virtual void OnClientAdded(const ports::NodeName& from_node,
                                     const ports::NodeName& client_name,
                                     int channel) = 0;
    virtual void OnAcceptClient(const ports::NodeName& from_node,
                                      const ports::NodeName& client_name,
                                      int channel) = 0;
    virtual void OnEventMessage(const ports::NodeName& from_node,
                                const Protocol::Ptr& message) = 0;
    virtual void OnAcceptPeer(const ports::NodeName& from_node,
                              const ports::NodeName& token,
                              const ports::NodeName& peer_name,
                              const ports::PortName& port_name) = 0;
    virtual void OnChannelError(const ports::NodeName& name,
                                NodeChannel* channel) = 0;
  };

  static Ptr Create(Delegate* delegate,
                    ConnectionParams connection_params,
                    IOTaskRunner* io_task_runner) {
    return std::make_shared<NodeChannel>(delegate,
                                         connection_params,
                                         io_task_runner);
  }


  NodeChannel(Delegate* delegate,
              ConnectionParams connection_params,
              IOTaskRunner* io_task_runner);

  void Start();
  void Shutdown();

  void SetRemoteNodeName(const ports::NodeName& name);

  void AcceptInvitee(const ports::NodeName& inviter_name,
                     const ports::NodeName& token);
  void AcceptInvitation(const ports::NodeName& token,
                        const ports::NodeName& invitee_name);
  void AcceptPeer(const ports::NodeName& sender_name,
                  const ports::NodeName& token,
                  const ports::PortName& port_name);
  void AddClient(const ports::NodeName& client_name);
  void ClientAdded(const ports::NodeName& client_name,
                         int channel);
  void AcceptClient(const ports::NodeName& name,
                          int channel,
                          const uint64_t capabilities);
  void RequestPortMerge(const ports::PortName& connector_port_name,
                        const std::string& token);

  void SendChannelMessage(const Protocol::Ptr& message);

  // Channel::Delegate
  void OnChannelMessage(const Protocol::Ptr& protocol) override;
  void OnChannelError() override;

  void WriteChannelMessage(const Protocol::Ptr& message);

 private:
  Delegate* delegate_;

  base::MutexLock channel_lock_;
  std::unique_ptr<Channel> channel_ GUARDED_BY(channel_lock_);

  ports::NodeName remote_node_name_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_NODE_CHANNEL_H
