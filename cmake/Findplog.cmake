# Findplog.cmake
#
# A simple Find module for `find_package()`
include(FetchContent)
FetchContent_Declare(plog
                     GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
                     GIT_TAG b9740d4582f5b7ce538b771510c98d69e42c7f06)
FetchContent_GetProperties(plog)
if(NOT plog_POPULATED)
  FetchContent_Populate(plog)
  message(STATUS "PACKAGE plog source dir: ${plog_SOURCE_DIR}")
  message(STATUS "PACKAGE plog binary dir: ${plog_BINARY_DIR}")
  add_library(plog INTERFACE)
  target_include_directories(plog INTERFACE "${plog_SOURCE_DIR}/include")
endif()
