find_library(GLOG_LIBRARY NAMES glog PATHS
    ${THIRDPARTY_LIB_DIR}
    /usr/lib/           # linux (apt)
    /usr/local/lib/     # brew
    /opt/local/lib/     # macports
)

if(GLOG_LIBRARY)
    set(FOUND_GLOG TRUE)
endif()

mark_as_advanced(GLOG_LIBRARY)
