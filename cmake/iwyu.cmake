# include-what-you-use

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
    message(STATUS "Could not find the program include-what-you-use; skipping...")
else()
    message(STATUS "Performing tests with include-what-you-use")
    set_property(TARGET ${MAIN_TARGET_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
    message(STATUS "Performing tests with include-what-you-use - done")
endif()
