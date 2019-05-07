include(FetchContent)
fetchcontent_declare(anyoption
                     GIT_REPOSITORY https://github.com/hackorama/AnyOption.git
                     GIT_TAG 47ecc0a593528924f45040d4005f42e8f5a090dd)
fetchcontent_getproperties(anyoption)
if(NOT anyoption_POPULATED)
  fetchcontent_populate(anyoption)
  # add_subdirectory(${anyoption_SOURCE_DIR} ${anyoption_BINARY_DIR})
  message(STATUS "PACKAGE AnyOption source dir: ${anyoption_SOURCE_DIR}")
  message(STATUS "PACKAGE AnyOption binary dir: ${anyoption_BINARY_DIR}")
endif()
