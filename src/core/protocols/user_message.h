//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_USER_MESSAGE_H
#define MOJO_IMPL_USER_MESSAGE_H

#include "core/protocol.h"

#include "core/byte_array.h"
#include "core/ports/name.h"

namespace tit {
namespace mojo {

class UserMessage : public ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<UserMessage>;

  static Ptr Create() {
    return std::make_shared<UserMessage>();
  }

//  ports::PortName from_port_name() const { return from_port_name_; }
//  ports::PortName to_port_name() const { return to_port_name_; }
//  std::string data() const { return data_; }
//
//  void set_from_port_name(const ports::PortName& port_name) {
//    from_port_name_ = port_name;
//  }
//  void set_to_port_name(const ports::PortName& port_name) {
//    to_port_name_ = port_name;
//  }
//  void set_data(const std::string& data) {
//    data_ = data;
//  }

  // ProtocolInterface
  ByteArray::Ptr Encode() override {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint64(from_port_name_.v1);
    bt->writeFuint64(from_port_name_.v2);
    bt->writeFuint64(to_port_name_.v1);
    bt->writeFuint64(to_port_name_.v2);
    bt->writeStringF32(data_);
    bt->setPosition(0);
    return bt;
  }
  void Decode(const ByteArray::Ptr& bt) override {
    uint64_t v1, v2;
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    from_port_name_ = ports::PortName(v1, v2);
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    to_port_name_ = ports::PortName(v1, v2);
    data_ = bt->readStringF32();
  }

  MsgType type() override { return MsgType::kUserMessage; }
  ProtocolInterface::Ptr next_layer() override {
    return nullptr;
  }

  std::string ToString() {
    std::stringstream ss;
    ss << "[ from_port_name: " << from_port_name_
       << "[ to_port_name: " << to_port_name_
       << " data=" << data_
       << "]";
    return ss.str();
  }

 public:
  ports::PortName from_port_name_ { ports::kInvalidPortName };
  ports::PortName to_port_name_ { ports::kInvalidPortName };
  std::string data_;
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_USER_MESSAGE_H
