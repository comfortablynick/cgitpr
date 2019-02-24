cmake_minimum_required(VERSION 3.6)
project (gitpr)
set(CMAKE_CXX_STANDARD 17)

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# compile
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build settings
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


# executable
# set(SOURCE_FILES gitpr.cpp)

set(SOURCE_FILES
        src/Main.cc
        src/Repo.cc
        src/Repo.h
        src/RepoArea.cc
        src/RepoArea.h
        src/Utils.cc
        src/Utils.h
)
add_executable(gitpr ${SOURCE_FILES})
