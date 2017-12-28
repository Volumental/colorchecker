find_package(SuiteSparse REQUIRED)

# Either the FindSuiteSparse.cmake or this variable needs to be adapted to add
# all the necessary libraries to CHOLMOD_LIBRARIES variable or the
# SuiteSparse_LIBRARIES variable. There are differences between platforms in
# which libraries need to be linked.
set(SuiteSparse_LIBRARIES
    ${CSPARSE_LIBRARY} ${CHOLMOD_LIBRARIES}
    ${AMD_LIBRARY} ${CAMD_LIBRARY}
    ${COLAMD_LIBRARY} ${CCOLAMD_LIBRARY}
)

if(ARCH_LINUX)
    find_library(SuiteSparseConfig_LIBRARY suitesparseconfig)
    list(APPEND SuiteSparse_LIBRARIES ${SuiteSparseConfig_LIBRARY})
endif()

set(SuiteSparse_INCLUDE_DIRS ${CSPARSE_INCLUDE_DIR} ${CHOLMOD_INCLUDE_DIR})
