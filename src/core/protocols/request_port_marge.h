//
// Created by titto on 2022/5/10.
//

#ifndef MOJO_IMPL_REQUEST_PORT_MARGE_H
#define MOJO_IMPL_REQUEST_PORT_MARGE_H

#include "core/byte_array.h"
#include "core/protocol.h"
#include "core/ports/name.h"

namespace tit {
namespace mojo {

class RequestPortMergeProtocol : public ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<RequestPortMergeProtocol>;

  static Ptr Create() { return std::make_shared<RequestPortMergeProtocol>(); }

  ByteArray::Ptr Encode() override {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint64(connector_port_name_.v1);
    bt->writeFuint64(connector_port_name_.v2);
    bt->setPosition(0);
    return bt;
  }

  void Decode(const ByteArray::Ptr& bt) override {
    uint64_t v1, v2;
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    connector_port_name_ = ports::PortName(v1, v2);
  }

  ProtocolInterface::Ptr next_layer() override { return nullptr; }
  MsgType type() override { return MsgType::kRequestPortMerge; }

  std::string ToString() {
    std::stringstream ss;
    ss << "[ port_name_: " << connector_port_name_ << " ]";
    return ss.str();
  }

 public:
  ports::PortName connector_port_name_{ports::kInvalidPortName};
};

}  // namespace mojo
}  // namespace tit

#endif  // MOJO_IMPL_REQUEST_PORT_MARGE_H
