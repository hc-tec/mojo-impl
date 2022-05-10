//
// Created by titto on 2022/5/10.
//

#ifndef MOJO_IMPL_ACCEPT_INVITATION_H
#define MOJO_IMPL_ACCEPT_INVITATION_H

#include "core/byte_array.h"
#include "core/protocol.h"
#include "core/ports/name.h"

namespace tit {
namespace mojo {

class AcceptInvitationProtocol : public ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<AcceptInvitationProtocol>;

  static Ptr Create() { return std::make_shared<AcceptInvitationProtocol>(); }

  ByteArray::Ptr Encode() override {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint64(token_.v1);
    bt->writeFuint64(token_.v2);
    bt->writeFuint64(invitee_name_.v1);
    bt->writeFuint64(invitee_name_.v2);
    bt->setPosition(0);
    return bt;
  }

  void Decode(const ByteArray::Ptr& bt) override {
    uint64_t v1, v2;
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    token_ = ports::NodeName(v1, v2);
    v1 = bt->readFuint64();
    v2 = bt->readFuint64();
    invitee_name_ = ports::NodeName(v1, v2);
  }

  ProtocolInterface::Ptr next_layer() override { return nullptr; }
  MsgType type() override { return MsgType::kAcceptInvitation; }

  std::string ToString() {
    std::stringstream ss;
    ss << "[ port_name: " << token_
       << "invitee_name: " << invitee_name_ << " ]";
    return ss.str();
  }

 public:
  ports::NodeName token_ { ports::kInvalidNodeName };
  ports::NodeName invitee_name_ { ports::kInvalidNodeName };
};

}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_ACCEPT_INVITATION_H
