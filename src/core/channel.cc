//
// Created by titto on 2022/5/6.
//

#include "core/channel.h"

#include "serializer.h"

namespace tit {
namespace mojo {

void Channel::Shutdown() {
  if (closed_) return;
  ShutdownImpl();
  delegate_ = nullptr;
  closed_ = true;
}

std::string Channel::SerializeProtocol(const ProtocolInterface::Ptr& protocol) {
  return protocol->Encode()->toString();
}

std::string Channel::SerializeMessage(const ProtocolInterface::Ptr& message) {
  return message->Encode()->toString();
}

std::string Channel::Serialize(const ProtocolInterface::Ptr& protocol) {
  return SerializeProtocol(protocol);
}

Protocol::Ptr Channel::DeserializeProtocol(const std::string& raw_data) {
  Serializer s;
  s << raw_data;
  s.reset();
  s.offset(1);
  Protocol::Ptr protocol = Protocol::Create();
  protocol->Decode(s.getByteArray());
  return protocol;
}

void Channel::DeserializeMessage(Protocol::Ptr& message,
                                          const std::string& raw_data) {
  Serializer s;
  s << raw_data;
  s.reset();
  s.offset(1);

  message->Decode(s.getByteArray());
}

}  // namespace mojo
}  // namespace tit
