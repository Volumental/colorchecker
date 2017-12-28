option(USE_SSE41 "Build with SSE 4.1" ON)
option(USE_FMA "Build with FMA" OFF)
option(USE_AVX "Build with AVX" OFF)

mark_as_advanced(USE_FMA USE_AVX)

if(USE_SSE41)
    # SSE <4.1 is automatically enabled with -msse4.1
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse4.1")
endif(USE_SSE41)

if(USE_AVX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx")
endif(USE_AVX)

if(USE_FMA)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mfma")
endif(USE_FMA)
