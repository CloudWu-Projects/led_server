cmake_minimum_required(VERSION "3.17.2")


project("ffi-demo-cpp")

set(LIBRARY_NAME "demo")
set(LIBRARY_SOURCE_FILE "lib.cpp")
set(LIBRARY_HEADER_FILE "lib.h")

add_library("${LIBRARY_NAME}" SHARED "${LIBRARY_SOURCE_FILE}")