project(RawImageProcessingTools)
cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET (Libraries Core Pnm Dng DngCore)

add_subdirectory(core)
add_subdirectory(lib_pnm)
add_subdirectory(lib_dng)
add_subdirectory(T872_challenge)
add_subdirectory(LJ92_eval)