file(GLOB_RECURSE source
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(color_calibration ${source})

target_link_libraries(color_calibration
    common
    image_toolbox
    ${OpenCV_LIBS}
)

# Add test program for this library
build_tests_for_library(color_calibration ${source})

# clang
target_compile_options(color_calibration PRIVATE -Wno-double-promotion)

# gcc
target_compile_options(color_calibration PRIVATE -Wno-sign-conversion -Wno-conversion)
