include(FetchContent)
FetchContent_Declare(
	How2use
	GIT_REPOSITORY https://github.com/SigmaPark/How2use.git
	GIT_TAG v2.1.0
)
FetchContent_MakeAvailable(How2use)

if(MSVC)
	add_definitions(-D_WINDOWS_SOLUTION_)

	add_compile_options(/Zc:__cplusplus)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)

endif()

if(UNIX)
	add_definitions(-D_POSIX_SOLUTION_)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${INCLUDE_DIR}/*.hpp
)

add_executable(Sigma_test ${SRC_FILES})

target_include_directories(Sigma_test PRIVATE ${INCLUDE_DIR})
target_link_libraries(Sigma_test PRIVATE How2use_lib)