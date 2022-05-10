//
// Created by titto on 2022/5/8.
//

#include "handle_table.h"

#include "log/logging.h"

namespace tit {
namespace mojo {

MojoHandle HandleTable::AddDispatcher(const Dispatcher::Ptr& dispatcher) {
  base::MutexLockGuard g(lock_);
  if (next_available_handle_ == MOJO_HANDLE_INVALID)
    return MOJO_HANDLE_INVALID;
  MojoHandle handle = next_available_handle_++;
  auto result = handles_.insert(std::make_pair(handle, Entry(dispatcher)));
  if (!result.second) {
    LOG(ERROR) << "handle_map insert error";
    return MOJO_HANDLE_INVALID;
  }
  return handle;
}

Dispatcher::Ptr HandleTable::GetDispatcher(MojoHandle handle) {
  base::MutexLockGuard g(lock_);
  auto it = handles_.find(handle);
  if (it == handles_.end()) {
    return nullptr;
  }
  return it->second.dispatcher;
}
}  // namespace mojo
}  // namespace tit