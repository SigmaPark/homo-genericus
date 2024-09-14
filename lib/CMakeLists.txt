file(
	GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(Sigma_lib STATIC ${SRC_FILES})

target_include_directories(Sigma_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_features(Sigma_lib PRIVATE cxx_std_17)
target_compile_definitions(Sigma_lib PRIVATE SGM_CXX_STANDARD=2017)