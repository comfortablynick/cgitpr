# Findfmt.cmake
#
# NOTE: does not work
include(FetchContent)
FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                     GIT_TAG e9bab6d028f3fd2cd3184cc1621a27b5a5f748f8)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  message(STATUS "PACKAGE fmt source dir: ${fmt_SOURCE_DIR}")
  message(STATUS "PACKAGE fmt binary dir: ${fmt_BINARY_DIR}")
  # add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
  # target_link_options(fmt PUBLIC)
  # add_library(fmt INTERFACE)
  add_library(fmt STATIC)
  target_include_directories(fmt PUBLIC "${fmt_SOURCE_DIR}")
endif()
