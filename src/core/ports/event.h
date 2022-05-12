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

  template <class T>
  typename T::Ptr GetMessage() {
    return std::dynamic_pointer_cast<T>(message_);
  }

  template <class T>
  typename T::Ptr* GetMessagePtr() {
    return std::dynamic_pointer_cast<T>(message_);
  }

  ProtocolInterface::Ptr* GetMessage() {
    return &message_;
  }

  void AttachMessage(const ProtocolInterface::Ptr& message) {
    message_ = message;
  }

  void AttachMessage(ProtocolInterface::Ptr& message) {
    message_ = message;
  }

  const PortName& to_port() const { return to_port_; }
  const PortName& from_port() const { return from_port_; }

  void set_to_port(const PortName& port_name) { to_port_ = port_name; }
  void set_from_port(const PortName& from_port) { from_port_ = from_port; }
 private:
  PortName to_port_;
  PortName from_port_;
  ProtocolInterface::Ptr message_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_EVENT_H
