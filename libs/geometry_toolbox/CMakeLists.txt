FILE(GLOB source "*.cpp" "*.hpp")

add_library(geometry_toolbox ${source})

target_link_libraries(geometry_toolbox
    common
)

# Add test program for this library
build_tests_for_library(geometry_toolbox ${source})

# clang
target_and_test_compile_options(geometry_toolbox PRIVATE
    -Wno-deprecated
    -Wno-double-promotion
    -Wno-gnu-zero-variadic-macro-arguments
)

# gcc
target_and_test_compile_options(geometry_toolbox PRIVATE -Wno-conversion -Wno-sign-conversion)
