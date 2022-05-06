//
// Created by titto on 2022/5/6.
//

#include "core/hash.h"

namespace tit {
namespace mojo {

size_t HashInts64Impl(uint64_t value1, uint64_t value2) {
  uint32_t short_random1 = 842304669U;
  uint32_t short_random2 = 619063811U;
  uint32_t short_random3 = 937041849U;
  uint32_t short_random4 = 3309708029U;

  uint32_t value1a = static_cast<uint32_t>(value1 & 0xffffffff);
  uint32_t value1b = static_cast<uint32_t>((value1 >> 32) & 0xffffffff);
  uint32_t value2a = static_cast<uint32_t>(value2 & 0xffffffff);
  uint32_t value2b = static_cast<uint32_t>((value2 >> 32) & 0xffffffff);

  uint64_t product1 = static_cast<uint64_t>(value1a) * short_random1;
  uint64_t product2 = static_cast<uint64_t>(value1b) * short_random2;
  uint64_t product3 = static_cast<uint64_t>(value2a) * short_random3;
  uint64_t product4 = static_cast<uint64_t>(value2b) * short_random4;

  size_t hash64 = product1 + product2 + product3 + product4;

  if (sizeof(size_t) >= sizeof(uint64_t))
    return static_cast<size_t>(hash64);

  uint64_t odd_random = 1578233944LL << 32 | 194370989LL;
  uint32_t shift_random = 20591U << 16;

  hash64 = hash64 * odd_random + shift_random;
  size_t high_bits =
      static_cast<size_t>(hash64 >> (8 * (sizeof(uint64_t) - sizeof(uint64_t))));
  return high_bits;
}

size_t HashInts64(uint64_t value1, uint64_t value2) {
  return HashInts64Impl(value1, value2);
}

}  // namespace mojo
}  // namespace tit
