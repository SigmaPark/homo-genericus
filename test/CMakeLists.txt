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


file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(Sigma_test ${SRC_FILES})

target_link_libraries(Sigma_test PRIVATE Sigma_lib)
target_compile_features(Sigma_test PRIVATE cxx_std_17)
target_compile_definitions(Sigma_test PRIVATE SGM_CXX_STANDARD=2017)