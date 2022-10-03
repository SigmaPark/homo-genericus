#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_COMPILER gcc-10)
set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_definitions(-D_GPP_SOLUTION_)
add_compile_options(-Wno-psabi)

project(GPP_Solution)

set(SGM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../include)
set(EIGEN_DIR $ENV{EIGEN_DIR})

include_directories(
    ${SGM_INCLUDE_DIR}
    ${EIGEN_DIR}
)

add_executable(
    GPP_Solution
    ../WorldTest.cpp 
    GPP_main.cpp

    ${SGM_INCLUDE_DIR}/SGM/Specification/Specification.cpp
    ${SGM_INCLUDE_DIR}/SGM/Specification/Test_Specification.cpp

    ${SGM_INCLUDE_DIR}/SGM/Abbreviable/Test_Avatar.cpp
    ${SGM_INCLUDE_DIR}/SGM/Abbreviable/Test_Nullable.cpp
    ${SGM_INCLUDE_DIR}/SGM/Abbreviable/Test_Pinweight.cpp  
    ${SGM_INCLUDE_DIR}/SGM/Array/Test_Array.cpp
    ${SGM_INCLUDE_DIR}/SGM/Boomerang/Test_Boomerang.cpp
    ${SGM_INCLUDE_DIR}/SGM/Concurrency/Test_Concurrency.cpp
    ${SGM_INCLUDE_DIR}/SGM/Family/Test_Family.cpp
    ${SGM_INCLUDE_DIR}/SGM/Flag_Set/Test_Flag_Set.cpp
    ${SGM_INCLUDE_DIR}/SGM/Functor/Test_Functor.cpp
    ${SGM_INCLUDE_DIR}/SGM/Functor/Test_Parameters.cpp
    ${SGM_INCLUDE_DIR}/SGM/High_Templar/Test_High_Templar.cpp
    ${SGM_INCLUDE_DIR}/SGM/interface_Traits/Test_interface_Traits.cpp
    ${SGM_INCLUDE_DIR}/SGM/iterable/Test_iterable.cpp 
    ${SGM_INCLUDE_DIR}/SGM/List/Test_List.cpp
    ${SGM_INCLUDE_DIR}/SGM/Mathexpr/Test_Mathexpr.cpp
    ${SGM_INCLUDE_DIR}/SGM/Operators_of/Test_Operators_of.cpp
    ${SGM_INCLUDE_DIR}/SGM/Recursion/Test_Recursion.cpp
	${SGM_INCLUDE_DIR}/SGM/Span/Test_Span.cpp
	${SGM_INCLUDE_DIR}/SGM/Type_Analysis/Test_Type_Analysis.cpp

    ${SGM_INCLUDE_DIR}/S3D/Affine/Test_Affine.cpp
    ${SGM_INCLUDE_DIR}/S3D/Decomposition/Test_Decomposition.cpp
    ${SGM_INCLUDE_DIR}/S3D/Euclid/Test_Euclid.cpp
    ${SGM_INCLUDE_DIR}/S3D/Hamilton/Test_Hamilton.cpp
    ${SGM_INCLUDE_DIR}/S3D/Quaternion/Test_Quaternion.cpp
)
