//
// Created by titto on 2022/5/8.
//

#include "core.h"

namespace tit {
namespace mojo {

Core::Core() {
  handle_table_ = HandleTable::Create();
  node_controller_ = NodeController::Create();
}

}  // namespace mojo
}  // namespace tit