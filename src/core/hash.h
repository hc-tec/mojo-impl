//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_HASH_H
#define MOJO_IMPL_HASH_H

#include <cstdint>

#include "core/def.h"

namespace tit {
namespace mojo {

size_t HashInts64Impl(uint64_t value1, uint64_t value2);

size_t HashInts64(uint64_t value1, uint64_t value2) {
  return HashInts64Impl(value1, value2);
}

}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_HASH_H
