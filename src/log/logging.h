// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_LOG_LOGGING_H
#define TIT_LOG_LOGGING_H

#include <functional>

#include "log_stream.h"

namespace tit {

namespace log {


class Logging {
 public:

  enum LogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kFatal,
    kNumOfLevels,
  };

  Logging(const char* file, int line);

  Logging(const char* file, int line, int level);

  class LogMessageData;

  LogStream& stream();

  ~Logging();

 private:
  void Init(const char* file, int line, int level);

  void Flush();

  void SendToLog();

  LogMessageData* data_;

  Logging(const Logging&);
  void operator=(const Logging&);

};

struct LogMessageInfo {

  explicit LogMessageInfo(const char* const level,
                          const char* const fullname,
                          const char* const filename,
                          const int& line,
                          const int& thread_id)
      : level_(level),
        fullname_(fullname),
        filename_(filename),
        line_(line),
        thread_id_(thread_id) {}

  const char* const level_;
  const char* const fullname_;
  const char* const filename_;
  const int &line_;
  const int &thread_id_;
//  const LogMessageTime& time;
};


typedef std::function<void(
    LogStream& stream,
    const LogMessageInfo& l,
    void* data)>
    CustomPrefixCallback;

void InitTitLogging(const char* argv0);

void InitTitLogging(const char* argv0,
                    CustomPrefixCallback prefix_callback,
                    void* prefix_callback_data);

void ShutdownTitLogging();

}  // namespace log


}  // namespace tit

#define TIT_LOG_INFO tit::log::Logging( \
    __FILE__, __LINE__)

#define TIT_LOG_TRACE tit::log::Logging( \
    __FILE__, __LINE__, tit::log::Logging::kTrace)

#define TIT_LOG_DEBUG tit::log::Logging( \
    __FILE__, __LINE__, tit::log::Logging::kDebug)

#define TIT_LOG_WARN tit::log::Logging( \
    __FILE__, __LINE__, tit::log::Logging::kWarn)

#define TIT_LOG_ERROR tit::log::Logging( \
    __FILE__, __LINE__, tit::log::Logging::kError)

#define TIT_LOG_FATAL tit::log::Logging( \
    __FILE__, __LINE__, tit::log::Logging::kFatal)

#define LOG(level) TIT_LOG_ ## level.stream()

#endif  // TIT_LOG_LOGGING_H
