#ifndef TIT_BASE_THREADLOCALSINGLETON_H
#define TIT_BASE_THREADLOCALSINGLETON_H

#include "base/noncopyable.h"

#include <cassert>
#include <pthread.h>

namespace tit {
namespace base {
template <typename T>
class ThreadLocalSingleton {
 public:
  ThreadLocalSingleton() = delete;
  ~ThreadLocalSingleton() = delete;

  static T& instance() {
    if (!t_value_) {
      t_value_ = new T();
      deleter_.set(t_value_);
    }
    return *t_value_;
  }

  static T* pointer() { return t_value_; }

 private:
  DISALLOW_COPY_AND_ASSIGN(ThreadLocalSingleton);

  static void destructor(void* obj) {
    assert(obj == t_value_);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy;
    (void)dummy;
    delete t_value_;
    t_value_ = 0;
  }

  class Deleter {
   public:
    Deleter() { pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor); }

    ~Deleter() { pthread_key_delete(pkey_); }

    void set(T* newObj) {
      assert(pthread_getspecific(pkey_) == NULL);
      pthread_setspecific(pkey_, newObj);
    }

    pthread_key_t pkey_;
  };

  static __thread T* t_value_;
  static Deleter deleter_;
};

template <typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template <typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}  // namespace base

}  // namespace tit
#endif  // TIT_BASE_THREADLOCALSINGLETON_H
