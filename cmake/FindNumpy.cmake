if (NOT NUMPY_FOUND)
    if (NOT PYTHONINTERP_FOUND)
        find_package(PythonInterp 3.5)
    endif ()
    # Using python to find the correct numpy
    execute_process (
        COMMAND ${PYTHON_EXECUTABLE} -c import\ numpy\;\ print\(numpy.get_include\(\)\)\;
        ERROR_VARIABLE NUMPY_FIND_ERROR
        RESULT_VARIABLE NUMPY_FIND_RESULT
        OUTPUT_VARIABLE NUMPY_FIND_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if (NOT NUMPY_FIND_RESULT)
        set (NUMPY_INCLUDE_DIR ${NUMPY_FIND_OUTPUT})
    endif ()
    if (NUMPY_FIND_ERROR)
        message("Error when finding numpy: ${NUMPY_FIND_ERROR}")
    endif()
endif()
