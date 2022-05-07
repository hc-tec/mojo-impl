//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_LIBUV_IO_TASK_RUNNER_ADAPTER_H
#define MOJO_IMPL_LIBUV_IO_TASK_RUNNER_ADAPTER_H

#include <uv.h>
#include <cstdlib>

#include <map>

#include "core/io_task_runner.h"

namespace tit {
namespace mojo {

class LibuvIOTaskRunnerAdapter : public IOTaskRunner {
 public:

  LibuvIOTaskRunnerAdapter()
      : loop_(static_cast<uv_loop_t *>(malloc(sizeof(uv_loop_t)))),
        close_(0) {
    uv_loop_init(loop_);
  }

  ~LibuvIOTaskRunnerAdapter() {
    Close();
  }

  void Run() {
    uv_run(loop_, UV_RUN_DEFAULT);
  }

  void Close() {
    if (close_) return;
    uv_loop_close(loop_);
    free(loop_);
    loop_ = nullptr;
    close_ = 1;
  }

  static void OnLibuvFdActive(uv_poll_t* handle, int status, int events);

  static void OnLibuvFdClose(uv_handle_t* handle);

  // IOTaskRunner
  void AddFdEvent(int fd, IOEvent event) override;
  void DelFdEvent(int fd, IOEvent event) override;

 private:
  uv_loop_t* loop_;
  std::map<int, uv_poll_t*> handles_map_;
  char close_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_LIBUV_IO_TASK_RUNNER_ADAPTER_H
