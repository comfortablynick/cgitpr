cmake_minimum_required(VERSION 3.6)

# include("cmake/HunterGate.cmake")
# HunterGate(
#     URL "https://github.com/ruslo/hunter/archive/v0.23.120.tar.gz"
#     SHA1 "7f43361ab4142e7d0eb03488b4b654aad38cf0cb"
# )

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
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


# hunter_add_package(glog)
# hunter_add_package(gflags)
# find_package(glog CONFIG REQUIRED)
# find_package(gflags CONFIG REQUIRED)
# if(NOT EXISTS "${glog_LICENSES}") # one file expected
#   message(FATAL_ERROR "File not found: '${glog_LICENSES}'")
# endif()

# executable

set(SOURCE_FILES
        src/Main.cc
        src/Options.cc
        src/Options.hpp
        src/Repo.cc
        src/Repo.hpp
        src/RepoArea.cc
        src/RepoArea.hpp
        src/Utils.cc
        src/Utils.hpp
)

add_executable(cgitpr ${SOURCE_FILES})
# target_link_libraries(cgitpr glog::glog gflags)
target_link_libraries(cgitpr ${CONAN_LIBS})

install(TARGETS cgitpr DESTINATION $ENV{HOME}/bin)
