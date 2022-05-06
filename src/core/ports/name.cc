//
// Created by titto on 2022/5/6.
//

#include "name.h"

namespace tit {
namespace mojo {
namespace ports {

const PortName kInvalidPortName = {0, 0};

const NodeName kInvalidNodeName = {0, 0};

std::ostream& operator<<(std::ostream& stream, const Name& name) {
  std::ios::fmtflags flags(stream.flags());
  stream << std::hex << std::uppercase << name.v1;
  if (name.v2 != 0)
    stream << '.' << name.v2;
  stream.flags(flags);
  return stream;
}

}  // namespace ports
}  // namespace mojo
}  // namespace tit