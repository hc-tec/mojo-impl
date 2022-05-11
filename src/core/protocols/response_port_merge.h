//
// Created by titto on 2022/5/11.
//

#ifndef MOJO_IMPL_RESPONSE_MERGE_PORT_H
#define MOJO_IMPL_RESPONSE_MERGE_PORT_H

#include "core/byte_array.h"
#include "core/protocol.h"
#include "core/ports/name.h"
#include "core/ports/event.h"

namespace tit {
namespace mojo {

class ResponsePortMergeProtocol : public ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<ResponsePortMergeProtocol>;

  static Ptr Create() { return std::make_shared<ResponsePortMergeProtocol>(); }

  ByteArray::Ptr Encode() override {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint64(connector_port_name_.v1);
    bt->writeFuint64(connector_port_name_.v2);
    bt->writeFuint64(port_name_.v1);
    bt->writeFuint64(port_name_.v2);
    bt->setPosition(0);
    return bt;
  }

  void Decode(const ByteArray::Ptr& bt) override {
    uint64_t v1, v2;
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    connector_port_name_ = ports::PortName(v1, v2);
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    port_name_ = ports::PortName(v1, v2);
  }

  ProtocolInterface::Ptr next_layer() override { return nullptr; }
  MsgType type() override { return MsgType::kResponsePortMerge; }

  std::string ToString() {
    std::stringstream ss;
    ss << "[ connector_port_name: " << connector_port_name_ << " ]"
       << "port_name: " << port_name_;
    return ss.str();
  }

 public:
  ports::PortName connector_port_name_{ ports::kInvalidPortName };
  ports::PortName port_name_{ ports::kInvalidPortName };
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_RESPONSE_MERGE_PORT_H
