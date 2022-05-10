//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_PROTOCOL_H
#define MOJO_IMPL_PROTOCOL_H

#include <memory>
#include <sstream>
#include <string>

#include "core/byte_array.h"

namespace tit {
namespace mojo {

enum class MsgType : uint8 {
  kHeaderType,

  kAcceptInvitee,
  kAcceptInvitation,
  kEventMessage,
  kRequestPortMerge,
  kRequestIntroduction,
  kIntroduction,
  kBroadcastEvent,
  kAcceptPeer,
};

class ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<ProtocolInterface>;
  // Encode object into ByteArray, then it can transform to string
  // used to transfer in network
  virtual ByteArray::Ptr Encode() = 0;
  // Decode the ByteArray into Protocol,
  // then we can use the decoded data received
  virtual void Decode(const ByteArray::Ptr& bt) = 0;

  virtual Ptr next_layer() = 0;

  virtual MsgType type() = 0;
};

// private protocol for mojo ipc
// 1st byte is magic number
// 2nd byte is version number
// 3rd byte is message type
// 4 bytes from 4th to 8th is content length
// Other bytes is data
class Protocol : public ProtocolInterface {
 public:
  using Ptr = std::shared_ptr<Protocol>;

  Protocol() = default;

  Protocol(Protocol* pProtocol) {}

  template <class T>
  static Protocol::Ptr TransToProtocolPtr() {
    return std::make_shared<Protocol>(
        reinterpret_cast<Protocol*>(
            new T));
  }

  static constexpr uint8 kMagic = 0xcc;
  static constexpr uint8 kDefaultVersion = 0x01;
  // 7: header size
  // 4: used to represent data length
  static constexpr uint8 kBaseLen = 7 + 4;

  static Protocol::Ptr Create() {
    return std::make_shared<Protocol>();
  }

  static Protocol::Ptr Create(MsgType type, const std::string& content) {
    Protocol::Ptr proto = std::make_shared<Protocol>();
    proto->set_msg_type(type);
    proto->set_content(content);
    return proto;
  }

  uint8 magic() const { return magic_; }
  uint8 version() const { return version_; }
  MsgType msg_type() const { return static_cast<MsgType>(type_); }
  uint32_t content_length() const { return content_length_; }
  const std::string& content() const { return data_; }

  void set_magic(uint8 magic) { magic_ = magic; }
  void set_version(uint8 version) { version_ = version; }
  void set_msg_type(MsgType type) { type_ = static_cast<uint8>(type); }
  void set_content_length(uint32_t len) { content_length_ = len; }
  void set_content(const std::string& content) { data_ = content; }

  ByteArray::Ptr EncodeMeta() {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint8(magic_);
    bt->writeFuint8(version_);
    bt->writeFuint8(type_);
    bt->writeFuint32(data_.size());
    bt->setPosition(0);
    return bt;
  }

  // ProtocolInterface
  ByteArray::Ptr Encode() override {
    ByteArray::Ptr bt = std::make_shared<ByteArray>();
    bt->writeFuint8(magic_);
    bt->writeFuint8(version_);
    bt->writeFuint8(type_);
    bt->writeFuint32(data_.size());
    bt->writeStringF32(data_);
    bt->setPosition(0);
    return bt;
  }
  void DecodeMeta(const ByteArray::Ptr& bt) {
    magic_ = bt->readFuint8();
    version_ = bt->readFuint8();
    type_ = bt->readFuint8();
    content_length_ = bt->readFuint32();
  }
  void Decode(const ByteArray::Ptr& bt) override {
    DecodeMeta(bt);
//    magic_ = bt->readFuint8();
//    version_ = bt->readFuint8();
//    type_ = bt->readFuint8();
//    content_length_ = bt->readFuint32();
//    data_ = bt->readStringF32();
//    content_length_ = data_.size();
  }
  MsgType type() override { return MsgType::kHeaderType; }
  void set_next_layer(const ProtocolInterface::Ptr& next_layer) {
    next_layer_ = next_layer;
  }

  ProtocolInterface::Ptr next_layer() override {
    return next_layer_;
  }

  std::string ToString() {
    std::stringstream ss;
    ss << "[ magic=" << magic_ << " version=" << version_
       << " type=" << type_ << " length=" << content_length_
       << " content=" << data_ << " ]";
    return ss.str();
  }

 private:
  // We called following variables Header except for the data_ field.
  uint8 magic_ { kMagic };
  uint8 version_ { kDefaultVersion };
  uint8 type_ { static_cast<uint8>(MsgType::kEventMessage) };
  uint32_t content_length_ { 0 };
  std::string data_;
  ProtocolInterface::Ptr next_layer_;

};

}  // namespace mojo
}  // namespace tit

#endif //MOJO_IMPL_PROTOCOL_H
