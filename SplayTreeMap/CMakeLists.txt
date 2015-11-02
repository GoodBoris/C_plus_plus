cmake_minimum_required( VERSION 2.8 )

file(GLOB_RECURSE cmake_helpers ${CMAKE_CURRENT_LIST_DIR}/cmake_scripts/*.cmake)
foreach(cmake_helper ${cmake_helpers})
	include(${cmake_helper})
endforeach(cmake_helper)

project(Map CXX)

include_directories(
    include
)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_subdirectory(include)
add_subdirectory(src)

add_executable( ${PROJECT_NAME} ${SRCS} )