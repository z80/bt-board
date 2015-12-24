
project( "usbspi" )
# CMake
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
   set(CMAKE_BUILD_TYPE     Debug)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_COLOR_MAKEFILE ON)
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set( ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
add_subdirectory( "./3dparty" )
add_subdirectory( "./lib" )


