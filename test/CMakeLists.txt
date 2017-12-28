# -----------------------------------------------------------------------------
#
# UNIT TESTING
#
# 1. Automatic test discovery
#   - tests for each library will be automatically detected
#   - tests will be linked only against the library being tested
#
# 2. Boilerplate reduction
#   - with Boost unit tests, no need to write a main function
#   - with Boost unit tests, no need to register test functions
#
# 3. Test grouping
#   - tests are grouped by library
#   - tests are built if tests are enabled and the library is enabled
#
# 4. CMake integration
#   - tests can be run with "make test", "ninja test", "ctest", ...
#
# -----------------------------------------------------------------------------

macro(build_tests_for_library library_name)
    if (BUILD_TESTS)
        message(STATUS "Adding tests for ${library_name}")
        add_executable("test_${library_name}" ${ARGN} "${PROJECT_SOURCE_DIR}/test/main.cpp")
        if (BUILD_SHARED_LIBS)
            target_link_libraries("${library_name}"
                ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
            )
        endif (BUILD_SHARED_LIBS)
        target_link_libraries("test_${library_name}"
            ${library_name}
            ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )
        add_test(
            NAME "test_${library_name}"
            COMMAND "test_${library_name}" --logdir=logs/
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        )
    endif (BUILD_TESTS)
endmacro(build_tests_for_library)

macro(target_and_test_compile_options library_name scope)
    target_compile_options(${library_name} ${scope} ${ARGN})
    if (BUILD_TESTS)
        target_compile_options("test_${library_name}" ${scope} ${ARGN})
    endif (BUILD_TESTS)
endmacro(target_and_test_compile_options)
