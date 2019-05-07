include(FetchContent)
fetchcontent_declare(clipp
                     GIT_REPOSITORY https://github.com/muellan/clipp.git
                     GIT_TAG 2c32b2f1f7cc530b1ec1f62c92f698643bb368db)
fetchcontent_getproperties(clipp)
if(NOT clipp_POPULATED)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  fetchcontent_populate(clipp)
  add_subdirectory(${clipp_SOURCE_DIR} ${clipp_BINARY_DIR})
  message(STATUS "PACKAGE clipp source dir: ${clipp_SOURCE_DIR}")
  message(STATUS "PACKAGE clipp binary dir: ${clipp_BINARY_DIR}")
endif()
