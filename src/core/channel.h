//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_CHANNEL_H
#define MOJO_IMPL_CHANNEL_H

#include "core/connection_params.h"
#include "core/io_task_runner.h"
#include "core/protocol.h"
#include "core/protocols/user_message.h"

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

  static Channel* Create(Delegate* delegate,
                         ConnectionParams connection_params,
                         IOTaskRunner* io_task_runner);

  Channel(Delegate* delegate,
          ConnectionParams connection_params,
          IOTaskRunner* io_task_runner)
      : delegate_(delegate),
        io_task_runner_(io_task_runner),
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

  static std::string Serialize(const ProtocolInterface::Ptr& protocol);

  static std::string SerializeProtocol(const ProtocolInterface::Ptr& protocol);

  static std::string SerializeMessage(const ProtocolInterface::Ptr& message);

  static Protocol::Ptr DeserializeProtocol(const std::string& raw_data);

  static void DeserializeMessage(Protocol::Ptr& message,
                                          const std::string& raw_data);

 protected:
  Delegate* delegate_;
  IOTaskRunner* io_task_runner_;

 private:
  char closed_;
};

}  // namespace mojo
}  // namespace tit



#endif //MOJO_IMPL_CHANNEL_H
