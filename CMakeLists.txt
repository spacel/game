set(PROJECT_NAME spacel)
set(TARGET_NAME Spacel)

project(${PROJECT_NAME})

set(PROJECT_LABEL "${TARGET_NAME} Game")
set(PROJECT_LABEL_SHORT "${TARGET_NAME}")

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
	)
set(BAKED_CMAKE_SOURCE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Urho3D
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(JSONCPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/include")
set(JSONCPP_LIBRARY jsoncpp)
add_subdirectory(lib/jsoncpp)

configure_file(src/project_defines.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/project_defines.h)

add_subdirectory(src/common)
add_subdirectory(src/client)
