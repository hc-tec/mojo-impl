// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Titto (2598772546@qq.com)

#ifndef TIT_BASE_NONCOPYABLE_H
#define TIT_BASE_NONCOPYABLE_H

// A macro to disallow the evil copy constructor and operator= functions
// This should be used in the private: declarations for a class
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)
#endif

#endif  // TIT_BASE_NONCOPYABLE_H
