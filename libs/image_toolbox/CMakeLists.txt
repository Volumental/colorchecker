FILE(GLOB source "*.cpp" "*.hpp")

add_library(image_toolbox ${source})

target_link_libraries(image_toolbox
    common
    file_io_toolbox
    geometry_toolbox
    ${OpenCV_LIBS}
)

# Add test program for this library
build_tests_for_library(image_toolbox ${source})

# clang
target_and_test_compile_options(image_toolbox PRIVATE -Wno-double-promotion -Wno-float-equal)

# gcc
target_and_test_compile_options(image_toolbox PRIVATE -Wno-conversion)
