//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_HASH_H
#define MOJO_IMPL_HASH_H

#include "base/def.h"

namespace tit {
namespace mojo {

size_t HashInts64Impl(uint64_t value1, uint64_t value2);

size_t HashInts64(uint64_t value1, uint64_t value2);

}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_HASH_H
