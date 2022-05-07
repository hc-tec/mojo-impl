//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_CHANNEL_H
#define MOJO_IMPL_CHANNEL_H

#include "core/protocol.h"
#include "core/ports/user_message.h"

namespace tit {

namespace mojo {

// This class is responsible for send data and read data from underlying object
// (e.g socket, shared memory). The data maybe need to transform format,
// it will be serialized when sending data or be deserialized when reading data
// completely.
class Channel {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnChannelMessage(const Protocol::Ptr& protocol) = 0;
    virtual void OnChannelError() = 0;
  };

  static Channel* Create(Delegate* delegate);

  Channel(Delegate* delegate)
      : delegate_(delegate),
        closed_(false) {}

  void Shutdown();

  virtual ~Channel() {
    Shutdown();
  }

  virtual void Start() = 0;

  virtual void Read() = 0;

  virtual void Write(const Protocol::Ptr& protocol) = 0;

  virtual void ShutdownImpl() = 0;

 public:

  static std::string Serialize(const Protocol::Ptr& protocol);

  static std::string SerializeProtocol(const Protocol::Ptr& protocol);

  static std::string SerializeMessage(const ports::UserMessage::Ptr& message);

  static Protocol::Ptr DeserializeProtocol(const std::string& raw_data);

  static ports::UserMessage::Ptr
  DeserializeMessage(const std::string& raw_data);

 private:

  Delegate* delegate_;
  char closed_;
};

}  // namespace mojo

}  // namespace tit



#endif //MOJO_IMPL_CHANNEL_H
