# Client recipe

set(URHO3D_HOME ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Urho3D/build)
set(URHO3D_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../assets)

# Find Urho3D library
include(Urho3D-CMake-common)
find_package(Urho3D REQUIRED)
include_directories(
	${URHO3D_INCLUDE_DIRS}
	${KNET_INCLUDE_DIR}
	..
	)

set(CLIENT_LIBRARIES
	${PROJECT_NAME}lib
	)

if (WIN32)
else ()
	find_package(X11 REQUIRED)
	find_package(OpenGL REQUIRED)
	set(CLIENT_LIBRARIES
		${CLIENT_LIBRARIES}
		Urho3D
		dl
		${X11_LIBRARIES}
		${OPENGL_LIBRARIES}
		${JSONCPP_LIBRARY}
		${SQLITE3_LIBRARY}
		${KNET_LIBRARY}
		)
endif ()

set(client_sources
	network/clientpackethandler.cpp
	ui/ModalWindow.cpp
	ui/ProgressBar.cpp
	client.cpp
	genericmenu.cpp
	game.cpp
	loadingscreen.cpp
	mainmenu.cpp
	settings.cpp
	spacelgame.cpp
	uievents.cpp)

# Hack due to the current cmake implementation of Urho3D library
remove_definitions(-DURHO3D_SSE -DURHO3D_OPENGL)

link_directories(${URHO3D_HOME}/lib
		${URHO3D_HOME}/Source/ThirdParty/SQLite
		${URHO3D_HOME}/Source/ThirdParty/kNet)
add_executable(${PROJECT_NAME} ${client_sources})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}lib)

target_link_libraries(${PROJECT_NAME} ${CLIENT_LIBRARIES})
