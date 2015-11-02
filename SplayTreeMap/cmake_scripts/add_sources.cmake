# http://stackoverflow.com/questions/7046956/populating-srcs-from-cmakelists-txt-in-subdirectories
#
# The macro first computes the path of the source file relative to the project root for each argument. 
# If the macro is invoked from inside a project sub directory the new value of the variable SRCS needs 
# to be propagated to the parent folder by using the PARENT_SCOPE option.

# This macro adds files in ${SRCS_VAR} (SRCS by default).
macro (add_sources)
    if (NOT DEFINED SRCS_VAR)
        set(SRCS_VAR SRCS)
    endif()
    file (RELATIVE_PATH _relPath ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach (_src ${ARGN})
        if (_relPath)
            set(_srcPath ${_relPath}/${_src})
        else()
            set(_srcPath ${_src})
        endif()
        list(APPEND ${SRCS_VAR} ${_srcPath})
    endforeach()
    unset(_srcPath)
    if (_relPath)
        # propagate SRCS to parent directory
        set (${SRCS_VAR} ${${SRCS_VAR}} PARENT_SCOPE)
    endif()
endmacro()