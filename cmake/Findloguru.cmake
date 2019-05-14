include(FetchContent)
set(pkg loguru)
FetchContent_Declare(${pkg}
                     # GIT_REPOSITORY https://github.com/emilk/loguru.git
                     # GIT_TAG 862ed46e48cd58a9c82a076cfd99a9f14cc6e622
                     GIT_REPOSITORY https://github.com/joonatoona/loguru.git
                     GIT_TAG 5e18155e34ea1df3ae93e0d56c9f8b144f32fa70)
FetchContent_GetProperties(${pkg})
if(NOT ${pkg}_POPULATED)
  FetchContent_Populate(${pkg})
  message(
    STATUS
      "${BoldCyan}PACKAGE ${pkg} source dir: ${${pkg}_SOURCE_DIR}${ColorReset}")
  message(
    STATUS
      "${BoldCyan}PACKAGE ${pkg} binary dir: ${${pkg}_BINARY_DIR}${ColorReset}")
  include_directories("${${pkg}_SOURCE_DIR}")
  add_library(${pkg} STATIC ${${pkg}_SOURCE_DIR}/loguru.cpp)
  target_link_options(${pkg}
                      PUBLIC
                      -pthread
                      -lpthread
                      -ldl)
endif()

# loguru compile options
set(USING_LOGURU 1)
set(LOGURU_WITH_STREAMS 0)
set(LOGURU_USE_FMTLIB 1)
set(LOGURU_REPLACE_GLOG 0)
set(LOGURU_FILENAME_WIDTH 16)
set(LOGURU_THREADNAME_WIDTH 16)
set(LOGURU_VERBOSE_SCOPE_ENDINGS 1)
set(LOGURU_SCOPE_TIME_PRECISION 3)

add_definitions(-DLOGURU_WITH_STREAMS=${LOGURU_WITH_STREAMS})
add_definitions(-DLOGURU_USE_FMTLIB=${LOGURU_USE_FMTLIB})
add_definitions(-DLOGURU_REPLACE_GLOG=${LOGURU_REPLACE_GLOG})
add_definitions(-DLOGURU_FILENAME_WIDTH=${LOGURU_FILENAME_WIDTH})
add_definitions(-DLOGURU_THREADNAME_WIDTH=${LOGURU_THREADNAME_WIDTH})
add_definitions(-DLOGURU_VERBOSE_SCOPE_ENDINGS=${LOGURU_VERBOSE_SCOPE_ENDINGS})
add_definitions(-DLOGURU_SCOPE_TIME_PRECISION=${LOGURU_SCOPE_TIME_PRECISION})

if(LOGURU_USE_FMTLIB)
  target_link_libraries(loguru fmt)
endif()
