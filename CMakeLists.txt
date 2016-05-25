set(PROJECT_NAME spacel)
set(TARGET_NAME Spacel)

project(${PROJECT_NAME})

cmake_minimum_required (VERSION 2.8.6)

set (CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
)
set (BAKED_CMAKE_SOURCE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Urho3D
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(src/client)
