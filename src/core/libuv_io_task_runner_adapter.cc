//
// Created by titto on 2022/5/6.
//

#include "libuv_io_task_runner_adapter.h"

#include "log/logging.h"

namespace tit {
namespace mojo {

struct uv_handle_data {
  LibuvIOTaskRunnerAdapter* adapter;
  int fd;
};

using uv_handle_data_t = struct uv_handle_data;

void LibuvIOTaskRunnerAdapter::OnLibuvFdActive(uv_poll_t* handle, int status, int events) {
  uv_handle_data_t* data = static_cast<uv_handle_data_t*>(handle->data);
  LibuvIOTaskRunnerAdapter* runner = data->adapter;
  if (status < 0) {
    LOG(INFO) << "libuv active error";
    return;
  }
  if (events | uv_poll_event::UV_READABLE) {
    runner->delegate()->OnFdReadable(data->fd);
  } else if (events | uv_poll_event::UV_WRITABLE) {
    runner->delegate()->OnFdWriteable(data->fd);
  }
}

void LibuvIOTaskRunnerAdapter::OnLibuvFdClose(uv_handle_t* handle) {
  uv_handle_data_t* data = static_cast<uv_handle_data_t*>(handle->data);
//  LibuvIOTaskRunnerAdapter* runner = data->adapter;
  LOG(INFO) << "libuv handle close, fd: " << data->fd;
  free(handle->data);
  free(handle);
//  runner->Close();
}

void LibuvIOTaskRunnerAdapter::AddFdEvent(int fd, IOEvent event) {
  uv_poll_t* handle_ =
      static_cast<uv_poll_t*>(malloc(sizeof(uv_poll_t)));
  uv_handle_data_t* data;
  data->adapter = this;
  data->fd = fd;
  handle_->data = data;
  handle_->close_cb = OnLibuvFdClose;
  uv_poll_init(loop_, handle_, fd);
  uv_poll_event uv_ev;
  if (event | IOEvent::kReadable) {
    uv_ev = uv_poll_event::UV_READABLE;
  } else if (event | IOEvent::kWritable) {
    uv_ev = uv_poll_event::UV_WRITABLE;
  }
  uv_poll_start(handle_, uv_ev, OnLibuvFdActive);
}

void LibuvIOTaskRunnerAdapter::DelFdEvent(int fd, IOEvent event) {
  uv_poll_t* handle_ =
      static_cast<uv_poll_t*>(malloc(sizeof(uv_poll_t)));
  uv_handle_data_t* data;
  data->adapter = this;
  data->fd = fd;
  handle_->data = data;
  handle_->close_cb = OnLibuvFdClose;
  uv_poll_init(loop_, handle_, fd);
  uv_poll_event uv_ev = uv_poll_event::UV_DISCONNECT;
  uv_poll_start(handle_, uv_ev, OnLibuvFdActive);
}


}  // namespace mojo
}  // namespace tit