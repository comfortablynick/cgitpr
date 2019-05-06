cmake_minimum_required(VERSION 3.8)

project(cgitpr
        VERSION "0.0.1"
        DESCRIPTION "Git status for your shell prompt"
        HOMEPAGE_URL "github.com/comfortablynick/cgitpr"
        LANGUAGES CXX)

# print cmake/system details
message(STATUS "CMake version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "Operating System: ${CMAKE_SYSTEM}")
message(STATUS "Processor:        ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)

set(USING_LOGURU 1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(compiler_check)
include(build_safeguards)

# find dependencies
if(USING_LOGURU)
  find_package(loguru)
  include_directories("${loguru_SOURCE_DIR}")
  add_library(loguru STATIC ${loguru_SOURCE_DIR}/loguru.cpp
                            ${loguru_SOURCE_DIR}/loguru.hpp)
  target_link_options(loguru
                      PUBLIC
                      -pthread
                      -lpthread
                      -ldl)
endif()

set(SOURCE_FILES
    src/main.cc
    src/repo.cc
    src/repo.h
    src/common.cc
    src/common.h)

# configure
message(STATUS "Running configure")
include(cmake/configure.cmake)
configure_file("${PROJECT_SOURCE_DIR}/cmake/config_h.cmake"
               "${PROJECT_SOURCE_DIR}/config.h" @ONLY)
message(STATUS "Running configure - done")

# MAIN_PROJECT_NAME allows us to use a name different from the project
set(MAIN_TARGET_NAME cgitpr)
add_executable(${MAIN_TARGET_NAME} ${SOURCE_FILES})
if(USING_LOGURU)
  target_link_libraries(${MAIN_TARGET_NAME} loguru) # ${CONAN_LIBS}
  set_target_properties(${MAIN_PROJECT_NAME}
                        PROPERTIES COMPILE_OPTIONS -pedantic -Wall -Wextra)
endif()

# check include-what-you-use
include(iwyu)

# install
install(TARGETS ${MAIN_TARGET_NAME} DESTINATION $ENV{HOME}/bin)
