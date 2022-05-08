//
// Created by titto on 2022/5/7.
//

#ifndef TIT_LOGGING_PROCESS_H
#define TIT_LOGGING_PROCESS_H

#include <unistd.h>

#include <string>

#include "log/logging.h"

namespace tit {
namespace base {

class Process;

extern Process* g_process;
extern std::string g_dir;
extern std::string g_execute_file;

void Init(int argc, char* argv[]);

Process* CurrentProcess();

std::string CurrentCommandLine();
std::string CurrentDirectory();
std::string CurrentExecuteName();

std::string _ArgValueParser(const std::string& arg);

class Process {
 public:

  void Init(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
      char* arg = argv[i];
      command_line_.append(arg);
      command_line_.append(" ");
    }
  }

  static Process* LaunchProcess(char* argument_list[]) {
    pid_t pid = fork();

    if (pid == 0) {
      setenv("PATH", CurrentDirectory().data(), 1);
//      std::string argument("-handler=");
//      argument.append(std::to_string(socket_pair[0]));
//      char* argument_list[] = {
//          const_cast<char*>(base::CurrentExecuteName().data()),
//          const_cast<char*>(argument.data()), NULL};
//      LOG(INFO) << "command line" << cmd_line.data();
//      LOG(INFO) << "channel: " << socket_pair[0];
      int r = execvp(argument_list[0], argument_list);
      if (r == -1) {
        LOG(INFO) << "execvp error";
      }
      _exit(127);
    }
    return new Process();
  }

  std::string command_line() { return command_line_; }

 private:
  std::string command_line_;
};

template<typename T>
struct ArgValueParser {
  T operator()(const std::string& arg) {
    std::string value = _ArgValueParser(arg);
    if (value.empty()) return "";
    return (T) value;
  }
};

template<>
struct ArgValueParser<int> {
  int operator()(const std::string& arg) {
    std::string value = _ArgValueParser(arg);
    if (value.empty()) return INTMAX_MAX;
    return atoi(value.data());
  }
};

}  // namespace base
}  // namespace tit


#endif  // TIT_LOGGING_PROCESS_H
