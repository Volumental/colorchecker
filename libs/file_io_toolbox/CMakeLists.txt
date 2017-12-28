FILE(GLOB source "*.cpp" "*.hpp")

add_library(file_io_toolbox ${source})

target_link_libraries(file_io_toolbox
    common
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

# gcc
target_compile_options(file_io_toolbox PUBLIC -Wno-float-conversion)
target_compile_options(file_io_toolbox PRIVATE -Wno-sign-conversion)
