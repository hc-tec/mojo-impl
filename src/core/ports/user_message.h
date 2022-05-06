//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_USER_MESSAGE_H
#define MOJO_IMPL_USER_MESSAGE_H

#include "core/byte_array.h"
#include "core/ports/name.h"

namespace tit {
namespace mojo {
namespace ports {

class UserMessage {
 public:
  using Ptr = std::shared_ptr<UserMessage>;

  PortName port_name() const { return port_name_; }
  std::string data() const { return data_; }

  ByteArray::Ptr Encode() {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint64(port_name_.v1);
    bt->writeFuint64(port_name_.v2);
    bt->writeStringF32(data_);
    bt->setPosition(0);
    return bt;
  }

  void Decode(const ByteArray::Ptr& bt) {
    uint64_t v1, v2;
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    port_name_ = PortName(v1, v2);
    data_ = bt->readStringF32();
  }
 private:
  PortName port_name_ { kInvalidPortName };
  std::string data_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_USER_MESSAGE_H
