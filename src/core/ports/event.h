//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_EVENT_H
#define MOJO_IMPL_EVENT_H

#include <memory>

#include "base/def.h"
#include "core/ports/name.h"
#include "core/protocol.h"

namespace tit {
namespace mojo {
namespace ports {

class Event {
 public:
  using Ptr = std::shared_ptr<Event>;

  enum class Type : uint8 {
    kUserMessage,
    kPortAccepted,
    kObserveProxy,
    kObserveClosure,
    kMergePort,
    kUserMessageReadAckRequest,
    kUserMessageReadAck,
  };

  struct PortDescriptor {
    NodeName peer_node_name;
    PortName peer_port_name;
    NodeName remote_node_name;
    PortName remote_port_name;
    uint64 next_sequence_num_to_send;
    uint64 next_sequence_num_to_receive;
    uint64 last_sequence_num_to_receive;
    bool peer_closed;
  };

  virtual ~Event();

  bool HasMessage() const { return !!message_; }
  void AttachMessage(const Protocol::Ptr& message) {
    message_ = message;
  }

  Type type() const { return type_; }

  const PortName& port_name() const { return port_name_; }
  const PortName& from_port() const { return from_port_; }

  void set_port_name(const PortName& port_name) { port_name_ = port_name; }
  void set_from_port(const PortName& from_port) { from_port_ = from_port; }
 private:
  Type type_;
  PortName port_name_;
  PortName from_port_;
  Protocol::Ptr message_;
};

class UserMessageEvent : public Event {
 public:
  using Ptr = std::shared_ptr<UserMessageEvent>;

 private:
//  std::vector<PortDescriptor> port_descriptors_;
//  std::vector<PortName> ports_;

};

}  // namespace ports
}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_EVENT_H
