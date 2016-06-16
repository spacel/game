set(unittests_sources
	main.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

add_executable(${PROJECT_NAME}unittests ${unittests_sources})

target_link_libraries(${PROJECT_NAME}unittests cppunit)
