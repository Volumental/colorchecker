FILE(GLOB source "*.cpp" "*.hpp")

add_library(common ${source})

find_package(Threads)

target_link_libraries(common
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBS}
    ${GFLAGS_LIB}
    ${OpenCV_LIBS}
    dl
    jemalloc
)

# Add test program for this library
build_tests_for_library(common ${source})

# Clang
target_and_test_compile_options(common PRIVATE
    -Wno-gnu-zero-variadic-macro-arguments
    -Wno-missing-variable-declarations
    -Wno-old-style-cast
)

# Clang
target_and_test_compile_options(common PUBLIC
    -Wno-double-promotion
    -Wno-gnu-zero-variadic-macro-arguments
    -Wno-range-loop-analysis
)
