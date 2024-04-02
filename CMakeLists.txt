cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_MINIMUM_VERSION 3.10)
project(Descartes LANGUAGES C CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(DESCARTES_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DESCARTES_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall") 
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g2")

execute_process(COMMAND ${CMAKE_COMMAND} .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/third_party)
if(RESULT)
    message(FATAL_ERROR "CMake step for download stage failed: ${RESULT}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/third_party)
if(result)
    message(FATAL_ERROR "Build step for third party stage failed: ${RESULT}")
endif()

add_subdirectory(src)
