find_path(NANOFLANN_INCLUDE_DIR nanoflann.hpp PATHS
    ${NANOFLANN_ROOT}
    ${THIRDPARTY_INCLUDE_DIR}
)
if (NANOFLANN_INCLUDE_DIR)
    set(NANOFLANN_FOUND True)
endif()
mark_as_advanced(
    NANOFLANN_INCLUDE_DIR
)
