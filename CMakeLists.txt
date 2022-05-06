cmake_minimum_required(VERSION 3.16)

project(mojo-impl)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


add_compile_options(-Wall -O0 -std=c++20 -g -Wno-sign-compare -Wno-class-memaccess -Wno-strict-aliasing)


include_directories(src)

link_libraries(uv)

add_subdirectory(src/core)
add_subdirectory(test)

link_directories(third_party)



#link_directories(third_party)

#include_directories(include/base)
#include_directories(include/log)


