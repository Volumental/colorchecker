# This module is for debugging purposes of the build system.
set(LIBRARY_VARIABLES
    ACCELERATION_FRAMEWORK
    Boost_LIBRARIES
    CHOLMOD_LIBRARIES
    CUDA_FRAMEWORK
    CUDA_LIBRARIES
    GLUT_glut_LIBRARY
    G2O_CORE_LIBRARY
    G2O_TYPES_SLAM3D
    G2O_STUFF_LIBRARY
    G2O_SOLVER_CSPARSE
    G2O_SOLVER_CSPARSE_EXTENSION
    OpenCV_LIBS
    OPENGL_gl_LIBRARY
    OPENGL_glu_LIBRARY
    OPENGL_FRAMEWORK
    OPENNI2_INCLUDE_DIR
    OPENNI2_LIBRARY
    PCL_LIBRARIES
    SuiteSparse_LIBRARIES
)

if(VERBOSE)
    message(STATUS "Library paths:")
    foreach(lib_name ${LIBRARY_VARIABLES})
        get_property(lib_value VARIABLE PROPERTY ${lib_name})
        message(STATUS "${lib_name}: ${lib_value}")
    endforeach()
endif(VERBOSE)
