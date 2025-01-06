cmake_minimum_required(VERSION 3.5)
project(HelloWorld)

file(GLOB SOURCES "src/*.cpp")
list (REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xwindowres.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xresources.cpp
)
add_executable(test ${SOURCES})
target_include_directories(test PRIVATE src aicodix/dsp aicodix/code aicodix)

IF (WIN32)
    SET(CMAKE_CXX_FLAGS "-DPLATFORM_WIN -DWITH_X=1")
ENDIF()