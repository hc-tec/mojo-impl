//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_UTIL_H
#define MOJO_IMPL_UTIL_H

#include <byteswap.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/time.h>

#include <bit>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <algorithm>
#include <iostream>

#include "log/logging.h"

namespace tit {

namespace mojo {

template<typename T>
constexpr T ByteSwap(T value) {
    if constexpr(sizeof(T) == sizeof(uint8_t)){
        return (T)bswap_16((uint16_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint16_t)){
        return (T)bswap_16((uint16_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint32_t)){
        return (T)bswap_32((uint32_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint64_t)){
        return (T)bswap_64((uint64_t)value);
    }
}

template<typename T>
constexpr T EndianCast(T value) {
  return value;
//    if constexpr(sizeof(T) == sizeof(uint8_t)
//                 || std::endian::native == std::endian::big){
//        return value;
//    } else if constexpr(std::endian::native == std::endian::little){
//        return ByteSwap(value);
//    }
}

}  // namespace mojo

}  // namespace tit

#endif //MOJO_IMPL_UTIL_H
