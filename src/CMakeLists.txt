cmake_minimum_required(VERSION 3.0)
project(nlzss11 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
  set(PYBIND11_CPP_STANDARD /std:c++latest)
else()
  set(PYBIND11_CPP_STANDARD -std=c++17)
endif()

add_subdirectory(./lib/pybind11)

pybind11_add_module(nlzss11
  common/binary_reader.h
  common/swap.h
  common/types.h
  main.cpp
  nlzss11.cpp
  nlzss11.h
)

target_include_directories(nlzss11 PRIVATE ./)

if (MSVC)
  target_compile_options(nlzss11 PRIVATE /W4)
else()
  target_compile_options(nlzss11 PRIVATE -Wall -Wextra)
endif()

target_include_directories(nlzss11 PUBLIC ./lib/tcb)
option(ZLIB_ENABLE_TESTS "" OFF)
add_subdirectory(./lib/zlib-ng)
target_link_libraries(nlzss11 PRIVATE zlib)
