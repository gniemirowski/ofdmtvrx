cmake_minimum_required(VERSION 3.5)
project(ofdmtvrx)

file(GLOB SOURCES "src/*.cpp")

set(RES_FILES "ofdmtvrx.rc")
set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

add_executable(ofdmtvrx ${SOURCES} ${RES_FILES})
target_include_directories(ofdmtvrx PRIVATE src aicodix/dsp aicodix/code aicodix)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

IF (WIN32)
    SET(CMAKE_CXX_FLAGS "-DPLATFORM_WIN")
ENDIF()

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git describe --all --exact-match OUTPUT_VARIABLE TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
target_compile_definitions(ofdmtvrx PRIVATE "GIT_HASH=${HASH}")
target_compile_definitions(ofdmtvrx PRIVATE "GIT_TAG=${TAG}")
