# Prevent the usage of ccache for CUDA targets on Linux, because ccache does
# not support the supplied parameter -E (run preprocessor only).
# Do this first so the variable is visible in the cmake cache.
if(UNIX)
    set(cuda_c_compiler "/usr/bin/gcc")
    # CUDA does not support gcc>4.8
    if(GCC_VERSION VERSION_GREATER 4.8)
        set(cuda_c_compiler "/usr/bin/gcc-4.8")
    endif()
    set(CUDA_HOST_COMPILER ${cuda_c_compiler} CACHE FILEPATH "CUDA host compiler")
endif(UNIX)
find_package(CUDA)
if(CUDA_FOUND)
    set(CUDA_NVCC_FLAGS "-arch sm_20 -std=c++11")
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)
    include_directories(${CUDA_INCLUDE_DIRS})
else()
    message(WARNING "CUDA not found, run cmake with the following flags to make it compile: ")
    message(WARNING "cmake -DBUILD_COMBINED_POSE_TRACKER=OFF -DBUILD_KINFU_TOOLBOX=OFF -DBUILD_KOMBCUDA=OFF -DBUILD_MAPPING_TOOLBOX=OFF -DBUILD_KOMB_GUI=OFF -DBUILD_RECORDER=OFF -DBUILD_TRACK_POSES=OFF -DBUILD_TESTS=OFF ..")
endif()
mark_as_advanced(
    CUDA_HOST_COMPILER
    CUDA_SDK_ROOT_DIR
    CUDA_TOOLKIT_ROOT_DIR
    CUDA_BUILD_CUBIN
    CUDA_BUILD_EMULATION
    CUDA_VERBOSE_BUILD
)
