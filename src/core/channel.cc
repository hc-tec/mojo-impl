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

std::string Channel::SerializeProtocol(const Protocol::Ptr& protocol) {
  return protocol->Encode()->toString();
}

std::string Channel::SerializeMessage(const ports::UserMessage::Ptr& message) {
  return message->Encode()->toString();
}

std::string Channel::Serialize(const Protocol::Ptr& protocol) {
  return SerializeProtocol(protocol);
}

Protocol::Ptr Channel::DeserializeProtocol(const std::string& raw_data) {
  Serializer s;
  s << raw_data;
  s.reset();
  s.offset(1);
  Protocol::Ptr protocol = Protocol::Create();
  protocol->DecodeMeta(s.getByteArray());
  return protocol;
}

ports::UserMessage::Ptr Channel::DeserializeMessage(
    const std::string& raw_data) {
  Serializer s;
  s << raw_data;
  s.reset();
  s.offset(1);
  ports::UserMessage::Ptr message = ports::UserMessage::Create();
  message->Decode(s.getByteArray());
  return message;
}

}  // namespace mojo
}  // namespace tit
