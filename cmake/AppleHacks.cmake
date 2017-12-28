if(APPLE)
    add_definitions(-DBOOST_NO_RVALUE_REFERENCES)
    add_definitions(-DBOOST_NO_VARIADIC_TEMPLATES)
    find_library(ACCELERATION_FRAMEWORK Accelerate)
    find_library(OPENGL_FRAMEWORK OPENGL)

    if(EXISTS /opt/local/include)
        add_compile_options(-isystem /opt/local/include) # macports
    endif()

    if(EXISTS /usr/local/include)
        add_compile_options(-isystem /usr/local/include) # Homebrew
    endif()

    if(EXISTS /opt/local/lib)
        link_directories(/opt/local/lib) # macports
    endif()

    if(EXISTS /usr/local/lib/)
        link_directories(/usr/local/lib/) # Homebrew
    endif()

    # Don't align Eigen because we get a lot of SIGBUS.
    add_definitions(-DEIGEN_DONT_ALIGN)

    SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif(APPLE)
