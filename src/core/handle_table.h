//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_HANDLE_TABLE_H
#define MOJO_IMPL_HANDLE_TABLE_H

#include <memory>
#include <unordered_map>
#include <utility>

#include "base/mutex.h"
#include "base/noncopyable.h"
#include "core/def.h"
#include "core/types.h"
#include "core/dispatcher.h"

namespace tit {
namespace mojo {

class HandleTable {
 public:
  using Ptr = std::shared_ptr<HandleTable>;

  static Ptr Create() {
    return std::make_shared<HandleTable>();
  }

  HandleTable() = default;
  ~HandleTable() = default;

  MojoHandle AddDispatcher(const Dispatcher::Ptr& dispatcher);

  Dispatcher::Ptr GetDispatcher(MojoHandle handle);

  DISALLOW_COPY_AND_ASSIGN(HandleTable);
 private:

  struct Entry {
    Entry() = default;
    explicit Entry(Dispatcher::Ptr  dispatcher)
        : dispatcher(std::move(dispatcher)) {}
    Entry(const Entry& other) = default;
    ~Entry() = default;

    Dispatcher::Ptr dispatcher;
    bool busy { false };
  };

  using HandleMap = std::unordered_map<MojoHandle, Entry>;

  base::MutexLock lock_;
  HandleMap handles_;

  uint64 next_available_handle_ = 1;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_HANDLE_TABLE_H
