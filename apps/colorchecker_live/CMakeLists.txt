FILE(GLOB source
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(colorchecker_live ${source})

target_link_libraries(colorchecker_live
    common
    color_calibration
)

# clang
target_compile_options(colorchecker_live PRIVATE -Wno-shorten-64-to-32)
