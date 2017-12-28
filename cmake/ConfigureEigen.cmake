find_path(EIGEN_INCLUDE_DIR REQUIRED Eigen/Core PATHS
    /usr/local/Cellar/eigen/3.3.4/include/eigen3 # Mac: brew install eigen
    /usr/local/Cellar/eigen/3.3.3/include/eigen3 # Mac: brew install eigen
    ${EIGEN_ROOT}
    ${THIRDPARTY_INCLUDE_DIR}/eigen3
    ${THIRDPARTY_INSTALL_PREFIX}
    /opt/local/include/eigen3
)

if (EIGEN_INCLUDE_DIR)
    include_directories(SYSTEM ${EIGEN_INCLUDE_DIR})
    set(EIGEN_FOUND True)
endif()

mark_as_advanced(
    EIGEN_INCLUDE_DIR
    EIGEN_INCLUDE_DIRS
)
