//
// Created by titto on 2022/5/6.
//

#include "core/channel.h"

namespace tit {
namespace mojo {

void Channel::Shutdown() {
  if (closed_) return;
  ShutdownImpl();
  delegate_ = nullptr;
  closed_ = true;
}


}  // namespace mojo
}  // namespace tit

