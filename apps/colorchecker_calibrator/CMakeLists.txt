FILE(GLOB source
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(colorchecker_calibrator ${source})

target_link_libraries(colorchecker_calibrator
    common
    color_calibration
)

# clang
target_compile_options(colorchecker_calibrator PRIVATE -Wno-shorten-64-to-32)
