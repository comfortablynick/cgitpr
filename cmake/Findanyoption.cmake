include(FetchContent)
FetchContent_Declare(anyoption
                     GIT_REPOSITORY https://github.com/hackorama/AnyOption.git
                     GIT_TAG 47ecc0a593528924f45040d4005f42e8f5a090dd)
FetchContent_GetProperties(anyoption)
if(NOT anyoption_POPULATED)
  FetchContent_Populate(anyoption)
  message(STATUS "PACKAGE AnyOption source dir: ${anyoption_SOURCE_DIR}")
  message(STATUS "PACKAGE AnyOption binary dir: ${anyoption_BINARY_DIR}")
  include_directories("${anyoption_SOURCE_DIR}")
  add_library(anyoption STATIC ${anyoption_SOURCE_DIR}/anyoption.cpp)
endif()
