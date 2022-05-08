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
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnAcceptInvitee() = 0;
    virtual void OnAcceptInvitation() = 0;
    virtual void OnChannelError(const ports::NodeName& name,
                                NodeChannel* channel) = 0;
  };

  NodeChannel(Delegate* delegate,
              ConnectionParams connection_params,
              IOTaskRunner* io_task_runner);

  void Start();
  void Shutdown();

  void AcceptInvitee();
  void AcceptInvitation();

  void SendChannelMessage(const Protocol::Ptr& message);

  // Channel::Delegate
  void OnChannelMessage(const Protocol::Ptr& protocol) override;
  void OnChannelError() override;

 private:
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
