cmake_minimum_required(VERSION 3.5)

project(
	Sigma
	LANGUAGES CXX
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_TEST_PROJ "Build Sigma's test suite" ON)

add_subdirectory(lib)

if(BUILD_TEST_PROJ)
	add_subdirectory(test)
endif()