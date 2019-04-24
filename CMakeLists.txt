cmake_minimum_required(VERSION 3.8)

project (cgitpr)

# print cmake/system details
message(STATUS "CMake version:    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "Operating System: ${CMAKE_SYSTEM}")
message(STATUS "Processor:        ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_CXX_STANDARD 17)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# check for c++17/c++1x
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++1x" COMPILER_SUPPORTS_CXX1X)
if(COMPILER_SUPPORTS_CXX17)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX1X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

# compile
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)

# build settings
set(CMAKE_BUILD_TYPE RelWithDebInfo)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -pthread")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
        src/Main.cc
        src/Options.h
        src/Repo.cc
        src/Repo.h
        src/Utils.cc
        src/Utils.h
)

add_executable(cgitpr ${SOURCE_FILES})
target_link_libraries(cgitpr ${CONAN_LIBS})

# include-what-you-use
find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
  message(FATAL_ERROR "Could not find the program include-what-you-use")
endif()

set_property(TARGET cgitpr PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

# install
install(TARGETS cgitpr DESTINATION $ENV{HOME}/bin)
