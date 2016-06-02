PROJECT(sqlite3)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(sqlite3 STATIC src/sqlite3.c src/sqlite3.h src/sqlite3ext.h)
# don't compile executable & shell.c

SET( SQLITE3_INCLUDE_DIR src )

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
