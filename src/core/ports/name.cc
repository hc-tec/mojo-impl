//
// Created by titto on 2022/5/6.
//

#include "name.h"

namespace tit {
namespace mojo {
namespace ports {

const PortName kInvalidPortName = {0, 0};

const NodeName kInvalidNodeName = {0, 0};

log::LogStream& operator<<(log::LogStream& stream, Name name) {
  stream << &std::hex << &std::uppercase << name.v1;
  if (name.v2 != 0)
    stream << '.' << name.v2;
  return stream;
}

std::ostream& operator<<(std::ostream& stream, Name name) {
  stream << &std::hex << &std::uppercase << name.v1;
  if (name.v2 != 0)
    stream << '.' << name.v2;
  return stream;
}

}  // namespace ports
}  // namespace mojo
}  // namespace tit
