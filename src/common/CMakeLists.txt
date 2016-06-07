set(common_sources
	config.cpp
	porting.cpp
	engine/server.cpp
	engine/space.cpp
	engine/databases/database-sqlite3.cpp
)

find_package(Urho3D REQUIRED)

include_directories(
	${URHO3D_INCLUDE_DIRS}
	${JSONCPP_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(${PROJECT_NAME}lib ${common_sources})

add_dependencies(${PROJECT_NAME}lib ${JSONCPP_LIBRARY} ${SQLITE3_LIBRARY})

target_link_libraries(${PROJECT_NAME}lib ${JSONCPP_LIBRARY} ${SQLITE3_LIBRARY})
