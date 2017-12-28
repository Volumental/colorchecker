find_library(GFLAGS_LIB  NAMES "gflags" PATHS
			${GFLAGS_ROOT}/lib
			${GFLAGS_ROOT}/Lib)

if (MSVC)
	find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h PATHS ${GFLAGS_ROOT}/Include)
	set(GFLAGS_LIB_DIR  ${GFLAGS_ROOT}/Lib)
else()
	find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h PATHS
		${GFLAGS_ROOT}/include)
	set(GFLAGS_LIB_DIR  ${GFLAGS_ROOT}/lib)
endif()
mark_as_advanced(
    GFLAGS_INCLUDE_DIR
    GFLAGS_LIB
)
