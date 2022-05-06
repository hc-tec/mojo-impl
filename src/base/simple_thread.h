// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_LOGGING_SIMPLE_THREAD_H
#define TIT_LOGGING_SIMPLE_THREAD_H

#include <functional>

#include "thread.h"

namespace tit {

namespace base {

class SimpleThread : public Thread {
 public:

  typedef std::function<void()> ThreadFunc;

  explicit SimpleThread(ThreadFunc, const std::string& = "");

  void ThreadMain() override;

 private:

  ThreadFunc func_;

};

}  // namespace base

}  // namespace tit



#endif  // TIT_LOGGING_SIMPLE_THREAD_H
