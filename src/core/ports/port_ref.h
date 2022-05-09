//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_PORT_REF_H
#define MOJO_IMPL_PORT_REF_H

#include <memory>

#include "core/ports/name.h"
#include "core/ports/port.h"

namespace tit {
namespace mojo {
namespace ports {

class PortRef {
 public:
  ~PortRef() = default;
  PortRef() = default;
  PortRef(const PortName& name, const Port::Ptr& port);

  PortRef(const PortRef& other) = default;
  PortRef(PortRef&& other) = default;

  PortRef& operator=(const PortRef& other) = default;
  PortRef& operator=(PortRef&& other) = default;

  const PortName& name() const { return name_; }
  Port::Ptr port() const { return port_; }

  bool is_valid() const { return !!port_; }

 private:

  PortName name_;
  Port::Ptr port_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_PORT_REF_H
