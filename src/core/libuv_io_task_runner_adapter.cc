//
// Created by titto on 2022/5/6.
//

#include "libuv_io_task_runner_adapter.h"

#include "log/logging.h"

namespace tit {
namespace mojo {

void LibuvIOTaskRunnerAdapter::OnLibuvFdActive(uv_poll_t* handle, int status, int events) {
  LibuvIOTaskRunnerAdapter* runner =
      static_cast<LibuvIOTaskRunnerAdapter*>(handle->data);
  if (status < 0) {
    LOG(INFO) << "libuv active error";
    return;
  }
  int fd = handle->u.fd;
  if (events & UV_READABLE) {
    runner->delegate(fd)->OnFdReadable(fd);
  } else if (events & UV_WRITABLE) {
    runner->delegate(fd)->OnFdWriteable(fd);
  }
}

void LibuvIOTaskRunnerAdapter::OnLibuvFdClose(uv_handle_t* handle) {
  LibuvIOTaskRunnerAdapter* runner =
      static_cast<LibuvIOTaskRunnerAdapter*>(handle->data);
  LOG(INFO) << "libuv handle close";
  free(handle);
  int fd = handle->u.fd;
  if (runner->handles_map_.find(fd) != runner->handles_map_.end()) {
    runner->handles_map_.erase(fd);
  }
//  runner->Close();
}

void LibuvIOTaskRunnerAdapter::AddFdEvent(int fd, IOEvent event) {
  uv_poll_t* handle_ = nullptr;
  if (handles_map_.find(fd) == handles_map_.end()) {
    handle_ =
        static_cast<uv_poll_t*>(malloc(sizeof(uv_poll_t)));
    handle_->data = this;
    handle_->close_cb = OnLibuvFdClose;
    handle_->u.fd = fd;
    uv_poll_init(loop_, handle_, fd);
    handles_map_[fd] = handle_;
  } else {
    handle_ = handles_map_[fd];
  }

  uv_poll_event uv_ev;
  if (event & IOEvent::kReadable) {
    uv_ev = UV_READABLE;
  } else if (event & IOEvent::kWritable) {
    uv_ev = UV_WRITABLE;
  }
  uv_poll_start(handle_, uv_ev, OnLibuvFdActive);
}

void LibuvIOTaskRunnerAdapter::DelFdEvent(int fd, IOEvent event) {
  uv_poll_t* handle_ = nullptr;
  if (handles_map_.find(fd) == handles_map_.end()) {
    return;
  } else {
    handle_ = handles_map_[fd];
  }

  uv_poll_event uv_ev = UV_DISCONNECT;
  uv_poll_start(handle_, uv_ev, OnLibuvFdActive);
}


}  // namespace mojo
}  // namespace tit