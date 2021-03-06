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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(compiler_check)
include(build_safeguards)

# find dependencies
find_package(fmt)
find_package(loguru)

set(SOURCE_FILES # all cpp files
    src/main.cc
    src/repo.cc
    src/common.cc)

# configure
message(STATUS "Running configure")
include(configure)
configure_file("${PROJECT_SOURCE_DIR}/cmake/config_h.cmake"
               "${PROJECT_SOURCE_DIR}/config.h" @ONLY)
message(STATUS "Running configure - done")

add_executable(cgitpr ${SOURCE_FILES})

target_link_libraries(cgitpr PUBLIC fmt loguru)

set_target_properties(cgitpr PROPERTIES COMPILE_OPTIONS -pedantic -Wall -Wextra)

# check include-what-you-use
include(iwyu)

# install
install(TARGETS cgitpr DESTINATION $ENV{HOME}/bin)
