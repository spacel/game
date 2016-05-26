set(common_sources
	config.cpp
	porting.cpp
)

find_package(Urho3D REQUIRED)

include_directories(
	${URHO3D_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIR}
)

add_library(${PROJECT_NAME}lib ${common_sources})

add_dependencies(${PROJECT_NAME}lib jsoncpp)
