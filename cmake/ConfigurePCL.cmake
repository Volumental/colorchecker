option(USE_SYSTEM_PCL "Whether to use the system installed pcl" OFF)

if (USE_SYSTEM_PCL)
    find_package(PCL REQUIRED)
else()
    find_package(PCL REQUIRED QUIET PATHS ${PCL_ROOT} ${THIRDPARTY_INSTALL_PREFIX} NO_DEFAULT_PATH)
endif()
mark_as_advanced(
    LIBUSB_1_INCLUDE_DIR
    LIBUSB_1_LIBRARY
    PCL_COMMON_INCLUDE_DIR
    PCL_COMMON_LIBRARY
    PCL_COMMON_LIBRARY_DEBUG
    PCL_DIR
    PCL_FEATURES_INCLUDE_DIR
    PCL_FEATURES_LIBRARY
    PCL_FEATURES_LIBRARY_DEBUG
    PCL_FILTERS_INCLUDE_DIR
    PCL_FILTERS_LIBRARY
    PCL_FILTERS_LIBRARY_DEBUG
    PCL_GEOMETRY_INCLUDE_DIR
    PCL_IO_INCLUDE_DIR
    PCL_IO_LIBRARY
    PCL_IO_LIBRARY_DEBUG
    PCL_KDTREE_INCLUDE_DIR
    PCL_KDTREE_LIBRARY
    PCL_KDTREE_LIBRARY_DEBUG
    PCL_KEYPOINTS_INCLUDE_DIR
    PCL_KEYPOINTS_LIBRARY
    PCL_KEYPOINTS_LIBRARY_DEBUG
    PCL_OCTREE_INCLUDE_DIR
    PCL_OCTREE_LIBRARY
    PCL_OCTREE_LIBRARY_DEBUG
    PCL_OUTOFCORE_INCLUDE_DIR
    PCL_OUTOFCORE_LIBRARY
    PCL_OUTOFCORE_LIBRARY_DEBUG
    PCL_REGISTRATION_INCLUDE_DIR
    PCL_REGISTRATION_LIBRARY
    PCL_REGISTRATION_LIBRARY_DEBUG
    PCL_SAMPLE_CONSENSUS_INCLUDE_DIR
    PCL_SAMPLE_CONSENSUS_LIBRARY
    PCL_SAMPLE_CONSENSUS_LIBRARY_DEBUG
    PCL_SEARCH_INCLUDE_DIR
    PCL_SEARCH_LIBRARY
    PCL_SEARCH_LIBRARY_DEBUG
    PCL_SEGMENTATION_INCLUDE_DIR
    PCL_SEGMENTATION_LIBRARY
    PCL_SEGMENTATION_LIBRARY_DEBUG
    PCL_SURFACE_INCLUDE_DIR
    PCL_SURFACE_LIBRARY
    PCL_SURFACE_LIBRARY_DEBUG
    PCL_TRACKING_INCLUDE_DIR
    PCL_TRACKING_LIBRARY
    PCL_TRACKING_LIBRARY_DEBUG
    PCL_VISUALIZATION_INCLUDE_DIR
    PCL_VISUALIZATION_LIBRARY
    PCL_VISUALIZATION_LIBRARY_DEBUG
    QHULL_INCLUDE_DIRS
    QHULL_LIBRARY
    QHULL_LIBRARY_DEBUG
)
