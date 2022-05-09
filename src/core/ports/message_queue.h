//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_MESSAGE_QUEUE_H
#define MOJO_IMPL_MESSAGE_QUEUE_H

#include <limits>
#include <vector>

#include "base/def.h"
#include "base/noncopyable.h"
#include "core/ports/event.h"

namespace tit {
namespace mojo {
namespace ports {

constexpr uint64_t kInitialSequenceNum = 1;
constexpr uint64_t kInvalidSequenceNum = std::numeric_limits<uint64_t>::max();

class MessageQueue {
 public:
  MessageQueue();
  explicit MessageQueue(uint64 next_sequence_num);
  ~MessageQueue();

  void set_signalable(bool val) { signalable_ = val; }

  uint64 next_sequence_num() const { return next_sequence_num_; }

  bool HasNextMessage() const;

  void GetNextMessage(UserMessageEvent::Ptr* message);

  void MessageProcessed();

  void AcceptMessage(UserMessageEvent::Ptr message,
                     bool* has_next_message);

  void TaskAllMessages(std::vector<UserMessageEvent::Ptr>* messages);

  size_t queued_message_count() const { return heap_.size(); }

  DISALLOW_COPY_AND_ASSIGN(MessageQueue);
 private:
  std::vector<UserMessageEvent::Ptr> heap_;
  uint64 next_sequence_num_;
  bool signalable_ { true };
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_MESSAGE_QUEUE_H
