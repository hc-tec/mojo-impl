//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_CORE_H
#define MOJO_IMPL_CORE_H

#include "core/io_task_runner.h"
#include "core/handle_table.h"
#include "core/node_controller.h"

namespace tit {
namespace mojo {

class Core {
 public:
  Core();

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;

  virtual ~Core();

  static Core* Get();

  void SetIOTaskRunner(IOTaskRunner* io_task_runner);

 private:
  HandleTable::Ptr handle_table_;
  NodeController::Ptr node_controller_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_CORE_H
