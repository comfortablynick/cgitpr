# Findfmt.cmake
include(FetchContent)
set(pkg fmt)
FetchContent_Declare(${pkg}
                     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                     GIT_TAG e9bab6d028f3fd2cd3184cc1621a27b5a5f748f8)
FetchContent_GetProperties(${pkg})
if(NOT ${pkg}_POPULATED)
  FetchContent_Populate(${pkg})
  message(
    STATUS
      "${BoldCyan}PACKAGE ${pkg} source dir: ${${pkg}_SOURCE_DIR}${ColorReset}")
  message(
    STATUS
      "${BoldCyan}PACKAGE ${pkg} binary dir: ${${pkg}_BINARY_DIR}${ColorReset}")
  add_subdirectory(${${pkg}_SOURCE_DIR} ${${pkg}_BINARY_DIR})
  target_link_options(${pkg} PUBLIC)
  include_directories("${${pkg}_SOURCE_DIR}")
endif()
