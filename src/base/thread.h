// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_BASE_THREAD_H
#define TIT_BASE_THREAD_H

#include <pthread.h>

#include <functional>
#include <memory>
#include <string>

#include "atomic_int.h"
#include "platform_thread.h"
#include "mutex.h"

namespace tit {

namespace base {

namespace {

static AtomicInt32 num_created_;

}  // namespace

class Thread : public PlatformThread::Delegate {
 public:

  explicit Thread(std::string name = "");

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  ~Thread() override;

  bool Start();
  void Stop();

  bool IsRunning() const {
    MutexLockGuard guard(running_lock_);
    return running_;
  }

  pid_t tid() const { return tid_; }
  const std::string& name() const { return name_; }

  static int NumCreated() {
    return num_created_.IncrementAndGet();
  };

  void ThreadMain() override;

  pthread_t& pthread() override;

  bool IsJoinable() override;

  void set_joinable(bool);

  int Join();

  int Detach();

 private:

  void SetDefaultName();

  bool stopping_;

  mutable MutexLock running_lock_;
  bool running_ GUARDED_BY(running_lock_);

  mutable MutexLock joinable_lock_;
  bool joinable_ GUARDED_BY(joinable_lock_);

  bool joined_;

  mutable MutexLock thread_lock_;
  pthread_t pthread_ GUARDED_BY(thread_lock_);

  pid_t tid_;
  std::string name_;
};

}  // namespace base

}  // namespace tit

#endif //TIT_BASE_THREAD_H
