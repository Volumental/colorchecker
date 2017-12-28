find_path(CERES_INCLUDE_DIR NAMES ceres/ceres.h PATHS
    ${THIRDPARTY_INCLUDE_DIR}
    /usr/include/       # linux (apt)
)
find_library(CERES_LIBRARY NAMES "ceres" PATHS
    ${THIRDPARTY_LIB_DIR}
    /usr/lib/           # linux (apt)
)

if(CERES_INCLUDE_DIR AND CERES_LIBRARY)
    set(FOUND_CERES TRUE)
endif()

mark_as_advanced(CERES_INCLUDE_DIR)
mark_as_advanced(CERES_LIBRARY)
