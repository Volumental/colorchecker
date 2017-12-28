# -----------------------------------------------------------------------------
# Default build mode and default flags
# -----------------------------------------------------------------------------

# CMake does not intialize the build type by default. Set it to release
# if no cache variable is present.
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

option(BUILD_SHARED_LIBS "Build all libraries shared" OFF)

set(CMAKE_CXX_STANDARD 14) # C++14
set(CMAKE_CXX_STANDARD_REQUIRED on)

if (NOT MSVC)
    # Uncomment to optimize debug build but still retain stack traces:
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -fno-optimize-sibling-calls")
endif()

# Use automatically architecture inference.
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    #Clang infers march automatically.
endif()

# Turn off Remotery profiler in Release builds:
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRMT_ENABLED=0")

# Make sure that RelWithDebInfo gets the same optimizations as Release
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3")

# Turn off debug_dumper in Release builds and on in Debug builds:
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DVOLUMENTAL_DEBUG_DUMPER=0")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DVOLUMENTAL_DEBUG_DUMPER=0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DVOLUMENTAL_DEBUG_DUMPER=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVOLUMENTAL_DEBUG_DUMPER=1")

# Turn off Loguru:s unsafe signal handler with stack traces in release, as it can lead to deadlocks:
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DLOGURU_UNSAFE_SIGNAL_HANDLER=0")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DLOGURU_UNSAFE_SIGNAL_HANDLER=0")

# -----------------------------------------------------------------------------
# Stricter build flags and warnings as error
# -----------------------------------------------------------------------------
# Fail compilation on any warning.
add_compile_options(-Werror)

# Turn on a lot of warnings:
add_compile_options(-Wall -Wextra -Wunreachable-code)

# Eigen 3.2 fails on these. Will be fixed in Eigen 3.3!
add_compile_options(-Wshadow -Wconversion -Wsign-conversion)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Turn off unused-local-typedefs. Eigen trips on these often.
    add_compile_options(-Wno-unused-local-typedefs)

    # It's well known that GCC has shortcomings about maybe-uninitialized.
    add_compile_options(-Wno-maybe-uninitialized)

    if (GCC_VERSION VERSION_GREATER 5.0)
        add_compile_options(-pedantic)
    endif() # GCC 5
endif() # GCC

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # We disable some Clang 3.7 warnings that older Clang version don't recognize:
    add_compile_options(-Wno-unknown-warning-option)

    # Turn on ALL warning for this and all future versions of Clang:
    add_compile_options(-Weverything)

    # Turn off specific warnings:
    add_compile_options(
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-disabled-macro-expansion
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-missing-noreturn
        -Wno-missing-prototypes
        -Wno-padded
        -Wno-reserved-id-macro
        -Wno-unused-macros
    )

    if(NOT APPLE)
        add_compile_options(-Wno-deprecated) # Eigen uses throw() instead of noexcept :(
    endif()

    # Clang has problems with e.g.  array<int, 3> = { 1, 2, 3 };
    add_compile_options(-Wno-missing-braces)

    # We use glu, which is deprecated on Mac:
    add_compile_options(-Wno-deprecated-declarations)

    add_compile_options(-Wno-error=unused-command-line-argument)

    # We very often define gflags, but never declare them. That is ok.
    add_compile_options(-Wno-missing-variable-declarations)

    # gflags issue:
    add_compile_options(-Wno-undefined-func-template)
endif() # Clang

set(EXTRA_LIBS "")

if (NOT FSANITIZER STREQUAL "")
    add_compile_options(
        -fno-omit-frame-pointer
        -fsanitize=${FSANITIZER}
    )

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(EXTRA_LIBS "-static-libasan -fsanitize=${FSANITIZER}")
    else()
        set(BUILD_SHARED_LIBS ON)
        if(APPLE)
            set(EXTRA_LIBS "-fsanitize=${FSANITIZER} -fPIC")
        else()
            set(EXTRA_LIBS "-lasan -fsanitize=${FSANITIZER} -fPIC")
        endif()
    endif()
endif()

if (NOT BUILD_SHARED_LIBS)
    # -fPIC is needed for Loguru for some unknown reason, possibly related to pthreads:
    add_compile_options(-fPIC)
endif()
