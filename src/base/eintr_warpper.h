// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This provides a wrapper around system calls which may be interrupted by a
// signal and return EINTR. See man 7 signal.
// To prevent long-lasting loops (which would likely be a bug, such as a signal
// that should be masked) to go unnoticed, there is a limit after which the
// caller will nonetheless see an EINTR in Debug builds.
//
// On Windows and Fuchsia, this wrapper macro does nothing because there are no
// signals.
//
// Don't wrap close calls in HANDLE_EINTR. Use IGNORE_EINTR if the return
// value of close is significant. See http://crbug.com/269623.

#ifndef BASE_POSIX_EINTR_WRAPPER_H_
#define BASE_POSIX_EINTR_WRAPPER_H_

#include <errno.h>

#define HANDLE_EINTR(x) ({ \
  decltype(x) eintr_wrapper_result; \
  do { \
    eintr_wrapper_result = (x); \
  } while (eintr_wrapper_result == -1 && errno == EINTR); \
  eintr_wrapper_result; \
})

#define HANDLE_EINTR(x) (x)
#define IGNORE_EINTR(x) (x)

#endif  // BASE_POSIX_EINTR_WRAPPER_H_