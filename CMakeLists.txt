cmake_minimum_required(VERSION 3.6)

project (cgitpr)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

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

install(TARGETS cgitpr DESTINATION $ENV{HOME}/bin)
