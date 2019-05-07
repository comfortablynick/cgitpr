include(FetchContent)
fetchcontent_declare(tclap
                     GIT_REPOSITORY https://github.com/mirror/tclap.git
                     GIT_TAG c51fb519d622ed6c41e05987fda8918bce689acd)
fetchcontent_getproperties(tclap)
if(NOT tclap_POPULATED)
  fetchcontent_populate(tclap)
  # add_subdirectory(${tclap_SOURCE_DIR} ${tclap_BINARY_DIR})
  message(STATUS "PACKAGE tclap source dir: ${tclap_SOURCE_DIR}")
  message(STATUS "PACKAGE tclap binary dir: ${tclap_BINARY_DIR}")
endif()
