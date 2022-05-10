//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_DISPATCHER_H
#define MOJO_IMPL_DISPATCHER_H

#include <memory>

#include "core/types.h"

namespace tit {
namespace mojo {

class Dispatcher {
 public:
  using Ptr = std::shared_ptr<Dispatcher>;

  enum class Type {
    kUnknown = 0,
    kMessagePipe,
    kDataPipeProducer,
    kDataPipeConsumer,
    kSharedBuffer,
    kWatcher,
    kInvitation
  };

  virtual Type GetType() const = 0;

  virtual MojoResult Close() = 0;

 protected:
  Dispatcher() = default;
  virtual ~Dispatcher() = default;

 private:

};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_DISPATCHER_H
