cmake_minimum_required(VERSION 3.22)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#                                      PROJECT FILE-STRUCTURE SETTINGS                                      #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

project(
  LANGUAGES CXX
)

if(WIN32)
  set(BUILD_SHARED_LIBS OFF) 
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)

include_directories(${CMAKE_SOURCE_DIR}/minmax)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#                                                TIC TAC TOE                                               #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

set(TIC_TAC_TOE_DEMO tic_tac_toe)
file(GLOB_RECURSE TIC_TAC_TOE_SRC ${CMAKE_SOURCE_DIR}/examples/tic_tac_toe.cpp)

add_executable(
  ${TIC_TAC_TOE_DEMO}
  ${TIC_TAC_TOE_SRC}
)

target_compile_options(
  ${TIC_TAC_TOE_DEMO}
  PRIVATE
  -O3
  -Wall
  -Wpedantic
  -Wfloat-equal
)
