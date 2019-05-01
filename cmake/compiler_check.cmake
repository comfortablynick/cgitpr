# check for c++17/c++1x

if(${CMAKE_CXX_STANDARD} EQUAL 17)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
  check_cxx_compiler_flag("-std=c++1x" COMPILER_SUPPORTS_CXX1X)
  if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  elseif(COMPILER_SUPPORTS_CXX1X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1x")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.
        Please use a different C++ compiler.")
  endif()
endif()
