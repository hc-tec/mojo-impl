//
// Created by titto on 2022/5/9.
//

#ifndef TIT_LOGGING_RAND_UTIL_H
#define TIT_LOGGING_RAND_UTIL_H

#include "log/logging.h"

namespace tit {
namespace base {

void RandBytes(void* output, size_t output_length);

}  // namespace base
}  // namespace tit

#endif  // TIT_LOGGING_RAND_UTIL_H
