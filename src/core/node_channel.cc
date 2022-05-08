//
// Created by titto on 2022/5/8.
//

#include "node_channel.h"

#include "core/channel_posix.h"

namespace tit {
namespace mojo {

NodeChannel::NodeChannel(NodeChannel::Delegate *delegate,
                         ConnectionParams connection_params,
                         IOTaskRunner *io_task_runner)
    : delegate_(delegate),
      channel_(Channel::Create(this,
                               connection_params,
                               io_task_runner)) {}

void NodeChannel::OnChannelMessage(const Protocol::Ptr& protocol) {}

void NodeChannel::OnChannelError() {}


}  // namespace mojo
}  // namespace tit