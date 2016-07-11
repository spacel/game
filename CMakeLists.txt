set(PROJECT_NAME spacel)
set(TARGET_NAME Spacel)

project(${PROJECT_NAME})

set(PROJECT_LABEL "${TARGET_NAME} Game")
set(PROJECT_LABEL_SHORT "${TARGET_NAME}")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROTOCOL_VERSION 1)

cmake_minimum_required(VERSION 2.8.6)

if(NOT CMAKE_BUILD_TYPE)
	# Default to release
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

set(BUILD_UNITTESTS TRUE CACHE BOOL "Build unittests")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules )
set(URHO3D_HOME ${CMAKE_CURRENT_SOURCE_DIR}/lib/Urho3D/build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -g -O0 -Wall -Wabi")

set(JSONCPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp/include")
set(JSONCPP_LIBRARY jsoncpp)
add_subdirectory(lib/jsoncpp)

set(SQLITE3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/Urho3D/Source/ThirdParty/SQLite/src")
set(SQLITE3_LIBRARY sqlite3)

set(KNET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/Urho3D/Source/ThirdParty/kNet/include")
set(KNET_LIBRARY kNet)

configure_file(src/project_defines.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/project_defines.h)

add_subdirectory(src/common)
add_subdirectory(src/client)
if(BUILD_UNITTESTS)
	add_subdirectory(src/unittests)
endif()
