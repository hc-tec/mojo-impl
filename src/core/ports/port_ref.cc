//
// Created by titto on 2022/5/8.
//

#include "port_ref.h"

namespace tit {
namespace mojo {
namespace ports {

PortRef::PortRef(const PortName& name, const Port::Ptr& port)
    : name_(name),
      port_(port) {}

}  // namespace ports
}  // namespace mojo
}  // namespace tit