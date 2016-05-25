set(client_sources
        main.cpp
)

if (WIN32)
else ()
    find_package(X11 REQUIRED)
    find_package(OpenGL REQUIRED)
    set (CLIENT_LIBRARIES
            Urho3D
            dl
            ${X11_LIBRARIES}
            ${OPENGL_LIBRARIES}
    )
endif()

link_directories(${URHO3D_HOME}/lib)
add_executable(${PROJECT_NAME} ${client_sources})
target_link_libraries(${PROJECT_NAME} ${CLIENT_LIBRARIES})
