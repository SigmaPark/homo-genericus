#	SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#	SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========

include(FetchContent)
FetchContent_Declare(
	How2use
	GIT_REPOSITORY https://github.com/SigmaPark/How2use.git
	GIT_TAG v2.1.0
)

set(BUILD_GUIDE_PROJ OFF CACHE BOOL "Disable How2use's guide project build" FORCE)
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


file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable(Sigma_test ${SRC_FILES})

target_link_libraries(Sigma_test PRIVATE Sigma_lib)
target_link_libraries(Sigma_test PRIVATE How2use_lib)