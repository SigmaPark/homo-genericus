#	SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#	SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(
	GLOB_RECURSE INCLUDE_FILES CONFIGURE_DEPENDS
	${INCLUDE_DIR}/*.hpp
)

add_library(Sigma_lib INTERFACE ${INCLUDE_FILES})

target_include_directories(Sigma_lib INTERFACE ${INCLUDE_DIR})
target_compile_features(Sigma_lib INTERFACE cxx_std_17)
target_compile_definitions(Sigma_lib INTERFACE SGM_CXX_STANDARD=2017)