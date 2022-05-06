// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_LOGGING_LOGGER_H
#define TIT_LOGGING_LOGGER_H

#include <cstdlib>
namespace tit {

namespace log {

class Logger {
 public:
  virtual ~Logger();


  virtual void Write(const char* message,
                     const size_t len) {
    Write(message, len, false);
  }

  // Writes "message[0,message_len-1]" corresponding to an event that
  // occurred at "timestamp".  If "force_flush" is true, the log file
  // is flushed immediately.
  virtual void Write(const char* message,
                     size_t len,
                     bool force_flush) = 0;

  // Flush any buffered messages
  virtual void Flush() = 0;

  // Get the current LOG file size.
  virtual u_int32_t LogSize() = 0;

};

}  // namespace log

}  // namespace tit



#endif  // TIT_LOGGING_LOGGER_H
