set(unittests_sources
	main.cpp
)

set(unitests_required_sources
	../client/settings.cpp
	../common/engine/generators.cpp
)

# Find Urho3D library
include(Urho3D-CMake-common)
find_package(Urho3D REQUIRED)
include_directories(
	${URHO3D_INCLUDE_DIRS}
	..
	../common
)

set(UNITTESTS_LIBRARIES
	${PROJECT_NAME}lib
	Urho3D
	dl
	cppunit
)

# Hack due to the current cmake implementation of Urho3D library
remove_definitions(-DURHO3D_SSE -DURHO3D_OPENGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")
link_directories(${URHO3D_HOME}/lib ${URHO3D_HOME}/Source/ThirdParty/SQLite)

add_executable(${PROJECT_NAME}unittests ${unittests_sources} ${unitests_required_sources})

target_link_libraries(${PROJECT_NAME}unittests ${UNITTESTS_LIBRARIES})
