find_library(GLFW_LIB NAMES "glfw3" PATHS
    ${THIRDPARTY_LIB_DIR}
    /opt/local/lib/
)

if(NOT GLFW_LIB)
    # On Mac we won't find glfw3, but we will find "glfw"
    find_library(GLFW_LIB NAMES "glfw" PATHS
        ${THIRDPARTY_LIB_DIR}
        /opt/local/lib/
    )
endif()

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h PATHS
    ${THIRDPARTY_INCLUDE_DIR}
    /opt/local/include/
)

if(UNIX AND NOT APPLE)
    set(GLFW_LIBRARIES
        "${GLFW_LIB}"

        # Needed for static linking:
        GL
        m
        rt
        X11
        Xcursor
        Xi
        Xinerama
        Xrandr
        Xxf86vm
    )
else()
    set(GLFW_LIBRARIES "${GLFW_LIB}")
endif()
