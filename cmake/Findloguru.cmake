include(FetchContent)
fetchcontent_declare(loguru
                     GIT_REPOSITORY https://github.com/emilk/loguru.git
                     GIT_TAG 862ed46e48cd58a9c82a076cfd99a9f14cc6e622)
fetchcontent_getproperties(loguru)
if(NOT loguru_POPULATED)
  fetchcontent_populate(loguru)
  message(STATUS "PACKAGE loguru source dir: ${loguru_SOURCE_DIR}")
  message(STATUS "PACKAGE loguru binary dir: ${loguru_BINARY_DIR}")
endif()

# loguru compile options
set(USING_LOGURU 1)
set(LOGURU_WITH_STREAMS 1)
set(LOGURU_REPLACE_GLOG 0)
set(LOGURU_FILENAME_WIDTH 23)
set(LOGURU_THREADNAME_WIDTH 16)
set(LOGURU_VERBOSE_SCOPE_ENDINGS 1)
set(LOGURU_SCOPE_TIME_PRECISION 3)

add_definitions(-DLOGURU_WITH_STREAMS=${LOGURU_WITH_STREAMS})
add_definitions(-DLOGURU_REPLACE_GLOG=${LOGURU_REPLACE_GLOG})
add_definitions(-DLOGURU_FILENAME_WIDTH=${LOGURU_FILENAME_WIDTH})
add_definitions(-DLOGURU_THREADNAME_WIDTH=${LOGURU_THREADNAME_WIDTH})
add_definitions(-DLOGURU_VERBOSE_SCOPE_ENDINGS=${LOGURU_VERBOSE_SCOPE_ENDINGS})
add_definitions(-DLOGURU_SCOPE_TIME_PRECISION=${LOGURU_SCOPE_TIME_PRECISION})
