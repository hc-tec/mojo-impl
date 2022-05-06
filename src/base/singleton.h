
#ifndef TIT_BASE_SINGLETON_H
#define TIT_BASE_SINGLETON_H

#include "noncopyable.h"

#include <cassert>
#include <pthread.h>
#include <cstdlib> // atexit

namespace tit {

namespace base {

namespace detail {
// This doesn't detect inherited member functions!
// http://stackoverflow.com/questions/1966362/sfinae-to-check-for-inherited-member-functions
template <typename T>
struct has_no_destroy {
  template <typename C>
  static char test(decltype(&C::no_destroy));
  template <typename C>
  static int32_t test(...);
  const static bool value = sizeof(test<T>(0)) == 1;
};
}  // namespace detail

template <typename T>
class Singleton {
 public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T& instance() {
    pthread_once(&ponce_, &Singleton::init);
    assert(value_ != NULL);
    return *value_;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Singleton);

  static void init() {
    value_ = new T();
    if (!detail::has_no_destroy<T>::value) {
      ::atexit(destroy);
    }
  }

  static void destroy() {
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy;
    (void)dummy;

    delete value_;
    value_ = NULL;
  }

 private:
  static pthread_once_t ponce_;
  static T* value_;
};

template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = NULL;

}  // namespace base
}  // namespace tit

#endif  // TIT_BASE_SINGLETON_H
