//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_NAME_H
#define MOJO_IMPL_NAME_H

#include <cstdint>
#include <ostream>
#include <tuple>

#include "core/hash.h"

namespace tit {
namespace mojo {
namespace ports {

struct Name {
  Name(uint64_t v1, uint64_t v2) : v1(v1), v2(v2) {}
  uint64_t v1, v2;
};

inline bool operator==(const Name& a, const Name& b) {
  return a.v1 == b.v1 && a.v2 == b.v2;
}

inline bool operator!=(const Name& a, const Name& b) {
  return !(a == b);
}

inline bool operator<(const Name& a, const Name& b) {
  return std::tie(a.v1, a.v2) < std::tie(b.v1, b.v2);
}

struct PortName : Name {
  PortName() : Name(0, 0) {}
  PortName(uint64_t v1, uint64_t v2) : Name(v1, v2) {}
};

struct NodeName : Name {
  NodeName() : Name(0, 0) {}
  NodeName(uint64_t v1, uint64_t v2) : Name(v1, v2) {}
};

extern const PortName kInvalidPortName;
extern const NodeName kInvalidNodeName;

}  // namespace ports
}  // namespace mojo
}  // namespace tit

namespace std {

template <>
struct hash<tit::mojo::ports::PortName> {
  uint64_t operator()(const tit::mojo::ports::PortName& name) const {
    return tit::mojo::HashInts64(name.v1, name.v2);
  }
};

template <>
struct hash<tit::mojo::ports::NodeName> {
  uint64_t operator()(const tit::mojo::ports::NodeName& name) const {
    return tit::mojo::HashInts64(name.v1, name.v2);
  }
};

}  // namespace std


#endif  // MOJO_IMPL_NAME_H
