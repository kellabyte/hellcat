cmake_minimum_required(VERSION 2.6)

# create_source_group(relativeSourcePath sourceGroupName files)
#
# Creates a source group with the specified name relative to the relative path
# specified.
#
# Parameters:
#    - sourceGroupName: Name of the source group to create.
#    - relativeSourcePath: Relative path to the files.
#    - sourceFiles: Files to add to the source group.
#
# For example if you have the following directory structure:
#
#    - ExampleApplication
#        - include
#            - Main.h
#                - Window
#                    Window.h
#        - source
#            - Main.cpp
#                - Window
#                    Window.cpp
#
# You can get your list of files and call create_source_group the following way
#
#    file(GLOB_RECURSE my_source_files ${CMAKE_CURRENT_SOURCE_DIR}/source/*)
#    create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/source" ${my_source_files})
#    file(GLOB_RECURSE my_header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
#    create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/include" ${my_header_files})
#    add_executable(ExampleApplication ${my_source_files} ${my_header_files})
#
# Then the generated solution would look like this
#
#    - ExampleApplication (project)
#        - Header Files
#            - Main.h
#                - Window
#                    Window.h
#        - Source Files
#            - Main.cpp
#                - Window
#                    Window.cpp
#
function(create_source_group sourceGroupName relativeSourcePath)
    FOREACH(currentSourceFile ${ARGN})
        FILE(RELATIVE_PATH folder ${relativeSourcePath} ${currentSourceFile})
        get_filename_component(filename ${folder} NAME)
        string(REPLACE ${filename} "" folder ${folder})
        if(NOT folder STREQUAL "")
            string(REGEX REPLACE "/+$" "" folderlast ${folder})
            string(REPLACE "/" "\\" folderlast ${folderlast})
            SOURCE_GROUP("${sourceGroupName}\\${folderlast}" FILES ${currentSourceFile})
        endif(NOT folder STREQUAL "")
    ENDFOREACH(currentSourceFile ${ARGN})
endfunction(create_source_group)

# ----------------------------------------
# hellcat executable
# ----------------------------------------
project(hellcat C CXX)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-mavx)
#add_definitions(-msse4.2)
add_definitions(-std=c++11)
add_definitions(-Weffc++)
add_definitions(-pedantic)
#add_definitions(-O3)
add_definitions(-Wall)
add_definitions(-Wextra)
add_definitions(-Wcast-align)
add_definitions(-w)

file(GLOB_RECURSE HELLCAT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

list(SORT HELLCAT_SOURCES)
create_source_group("Source Files" "${CMAKE_CURRENT_SOURCE_DIR}/src" ${HELLCAT_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/lib/libevent-2.0.21-stable/include/)
include_directories(${CMAKE_SOURCE_DIR}/lib/mdb/libraries/liblmdb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/Haywire/include/)

find_package(Threads REQUIRED)

add_executable (hellcat
    ${HELLCAT_SOURCES}
    ${CMAKE_SOURCE_DIR}/lib/mdb/libraries/liblmdb/mdb.o
    ${CMAKE_SOURCE_DIR}/lib/mdb/libraries/liblmdb/midl.o)

target_link_libraries (hellcat ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_SOURCE_DIR}/lib/Haywire/builds/unix/debug/libuv.a
    ${CMAKE_SOURCE_DIR}/lib/Haywire/builds/unix/debug/libhaywire.a)
