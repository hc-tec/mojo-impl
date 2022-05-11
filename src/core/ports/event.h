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

  static Ptr Create() {
    return std::make_shared<Event>();
  }

  bool HasMessage() const { return !!message_; }
  void AttachMessage(const ProtocolInterface::Ptr& message) {
    message_ = message;
  }

  const PortName& port_name() const { return port_name_; }
  const PortName& from_port() const { return from_port_; }

  void set_port_name(const PortName& port_name) { port_name_ = port_name; }
  void set_from_port(const PortName& from_port) { from_port_ = from_port; }
 private:
  PortName port_name_;
  PortName from_port_;
  ProtocolInterface::Ptr message_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_EVENT_H
