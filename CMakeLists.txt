cmake_minimum_required(VERSION 3.8)

project(cgitpr)

# print cmake/system details
message(STATUS "CMake version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "Operating System: ${CMAKE_SYSTEM}")
message(STATUS "Processor:        ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(compiler_check)
include(build_safeguards)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -pthread -Wno-unused-variable")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    src/Main.cc
    src/Options.h
    src/Repo.cc
    src/Repo.h
    src/Utils.cc
    src/Utils.h)

# MAIN_PROJECT_NAME allows us to use a name different from the project
set(MAIN_TARGET_NAME ${PROJECT_NAME})
add_executable(${MAIN_TARGET_NAME} ${SOURCE_FILES})
target_link_libraries(${MAIN_TARGET_NAME} ${CONAN_LIBS})

# check include-what-you-use
include(iwyu)

# install
install(TARGETS ${MAIN_TARGET_NAME} DESTINATION $ENV{HOME}/bin)
