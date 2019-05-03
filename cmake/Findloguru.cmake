include(FetchContent)
fetchcontent_declare(loguru
                     GIT_REPOSITORY https://github.com/emilk/loguru.git
                     GIT_TAG 862ed46e48cd58a9c82a076cfd99a9f14cc6e622)
fetchcontent_getproperties(loguru)
message(STATUS "PACKAGE loguru source dir: ${loguru_SOURCE_DIR}")
message(STATUS "PACKAGE loguru binary dir: ${loguru_BINARY_DIR}")
if(NOT loguru_POPULATED)
  fetchcontent_populate(loguru)
endif()
