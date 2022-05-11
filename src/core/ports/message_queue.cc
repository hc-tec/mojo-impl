//
// Created by titto on 2022/5/8.
//

#include "message_queue.h"

namespace tit {
namespace mojo {
namespace ports {

MessageQueue::MessageQueue()
    : next_sequence_num_(kInitialSequenceNum) {}

MessageQueue::MessageQueue(uint64 next_sequence_num)
    : next_sequence_num_(next_sequence_num) {}

MessageQueue::~MessageQueue() {}

bool MessageQueue::HasNextMessage() const {
  return !heap_.empty();
}

void MessageQueue::GetNextMessage(Event::Ptr *message) {
//  std::pop_heap(heap_.begin(), heap_.end());
  *message = std::move(heap_.back());
  heap_.pop_back();

  constexpr size_t kHeapMinimumShrinkSize = 16;
  constexpr size_t kHeapShrinkInterval = 512;
  if (unlikely(heap_.size() > kHeapMinimumShrinkSize &&
               heap_.size() % kHeapShrinkInterval == 0)) {
    heap_.shrink_to_fit();
  }
}

void MessageQueue::MessageProcessed() {
  ++next_sequence_num_;
}

void MessageQueue::AcceptMessage(Event::Ptr message,
                                 bool *has_next_message) {
  heap_.emplace_back(std::move(message));
  if (!signalable_) {
    *has_next_message = false;
  } else {
    *has_next_message = true;
  }
}

void MessageQueue::TaskAllMessages(std::vector<Event::Ptr> *messages) {
  *messages = std::move(heap_);
}

}  // namespace ports
}  // namespace mojo
}  // namespace tit