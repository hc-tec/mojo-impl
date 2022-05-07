//
// Created by titto on 2022/5/7.
//

#ifndef TIT_LOGGING_PROCESS_H
#define TIT_LOGGING_PROCESS_H

#include <string>

namespace tit {
namespace base {

class Process;

extern Process* g_process;
extern std::string g_dir;
extern std::string g_execute_file;

class Process {
 public:

  void Init(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
      char* arg = argv[i];
      command_line_.append(arg);
      command_line_.append(" ");
    }
  }

  std::string command_lien() { return command_line_; }

 private:
  std::string command_line_;
};

void Init(int argc, char* argv[]);

Process* CurrentProcess();

std::string CurrentCommandLine();
std::string CurrentDirectory();
std::string CurrentExecuteName();

std::string _ArgValueParser(const std::string& arg);

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
