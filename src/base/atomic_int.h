// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_BASE_ATOMIC_H
#define TIT_BASE_ATOMIC_H

#include <cstdint>

namespace tit {

namespace base {

template <typename T>
class AtomicIntegerT {
 public:
  AtomicIntegerT()
      : value_(0) {}

  T Get()
  {
    // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
    return __sync_val_compare_and_swap(&value_, 0, 0);
  }

  T GetAndAdd(T x)
  {
    // in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
    return __sync_fetch_and_add(&value_, x);
  }

  T AddAndGet(T x)
  {
    return GetAndAdd(x) + x;
  }

  T IncrementAndGet()
  {
    return AddAndGet(1);
  }

  T DecrementAndGet()
  {
    return AddAndGet(-1);
  }

  void Add(T x)
  {
    GetAndAdd(x);
  }

  void Increment()
  {
    IncrementAndGet();
  }

  void Decrement()
  {
    DecrementAndGet();
  }

  T GetAndSet(T newValue)
  {
    // in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
    return __sync_lock_test_and_set(&value_, newValue);
  }

 private:
  volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

}  // namespace base

}  // namespace tit


#endif  // TIT_BASE_ATOMIC_H
