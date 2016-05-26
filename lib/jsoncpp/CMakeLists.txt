if( CMAKE_COMPILER_IS_GNUCXX )
  #Get compiler version.
  execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                   OUTPUT_VARIABLE GNUCXX_VERSION )

  #-Werror=* was introduced -after- GCC 4.1.2
  if( GNUCXX_VERSION VERSION_GREATER 4.1.2 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=strict-aliasing")
  endif()
endif( CMAKE_COMPILER_IS_GNUCXX )

SET( JSONCPP_INCLUDE_DIR include )

SET( PUBLIC_HEADERS
    ${JSONCPP_INCLUDE_DIR}/json/json.h
    ${JSONCPP_INCLUDE_DIR}/json/json-forwards.h
    )

SOURCE_GROUP( "Public API" FILES ${PUBLIC_HEADERS} )

SET(jsoncpp_sources
                jsoncpp.cpp)

# Install instructions for this target
IF(JSONCPP_WITH_CMAKE_PACKAGE)
    SET(INSTALL_EXPORT EXPORT jsoncpp)
ELSE(JSONCPP_WITH_CMAKE_PACKAGE)
    SET(INSTALL_EXPORT)
ENDIF()

ADD_LIBRARY(jsoncpp STATIC ${PUBLIC_HEADERS} ${jsoncpp_sources})
SET_TARGET_PROPERTIES( jsoncpp PROPERTIES VERSION ${JSONCPP_VERSION} SOVERSION ${JSONCPP_VERSION_MAJOR})
SET_TARGET_PROPERTIES( jsoncpp PROPERTIES OUTPUT_NAME jsoncpp
                      DEBUG_OUTPUT_NAME jsoncpp${DEBUG_LIBNAME_SUFFIX} )

IF(NOT CMAKE_VERSION VERSION_LESS 2.8.11)
   TARGET_INCLUDE_DIRECTORIES( jsoncpp PUBLIC
                          $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${JSONCPP_INCLUDE_DIR}>
                          )
ENDIF()

