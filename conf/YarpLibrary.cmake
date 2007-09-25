
MACRO(YarpLibrary name postfix)

PROJECT(lib${name})

FOREACH(X ${NEED_LIBS})
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/lib${X}/include")
ENDFOREACH(X ${NEED_LIBS})
SET(header_path ${CMAKE_SOURCE_DIR}/src/lib${name}/include)
INCLUDE_DIRECTORIES("${header_path}")

#AUX_SOURCE_DIRECTORY(src libcode)
FILE(GLOB_RECURSE folder_source src/*.cpp)
SET(libcode ${folder_source})
FILE(GLOB_RECURSE folder_header include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

#INCLUDE(YarpReqLib)

ADD_LIBRARY(${name} ${libcode} ${folder_header})
SET_TARGET_PROPERTIES(${name} PROPERTIES header_path ${header_path})

SET_TARGET_PROPERTIES(
  ${name}
PROPERTIES
  VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}
  SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}
)

# describe library information for an external user
GET_TARGET_PROPERTY(${name}_LIB ${name} LOCATION)
GET_TARGET_PROPERTY(${name}_INC ${name} header_path)
SET(${name}_LIB ${${name}_LIB} CACHE INTERNAL "libraries")
IF (ALT_INCLUDES)
	SET(${name}_INC "${ALT_INCLUDES}" CACHE INTERNAL "include path")
ELSE (ALT_INCLUDES)
	SET(${name}_INC "${${name}_INC}" CACHE INTERNAL "include path")
ENDIF (ALT_INCLUDES)
IF (WIN32 AND NOT CYGWIN)
   SET(libname "${${name}_LIB}")
   STRING(REPLACE ".lib" "d.lib" libname2 ${libname})
   SET(${name}_LIB optimized ${libname} debug ${libname2} CACHE INTERNAL "libraries")
ENDIF (WIN32 AND NOT CYGWIN)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_${postfix} ${harnesscode})

TARGET_LINK_LIBRARIES(harness_${postfix} ${name} ${NEED_LIBS} ${ACE_LINK_FLAGS})

GET_TARGET_PROPERTY(EXEC harness_${postfix} LOCATION)

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
IF(WIN32 AND NOT CYGWIN)
	  ADD_TEST(${name}::${XN} ${EXECUTABLE_OUTPUT_PATH}/harness_${postfix} regression ${XN})
ELSE(WIN32 AND NOT CYGWIN)
	  ADD_TEST(${name}::${XN} ${EXEC} regression ${XN})
ENDIF(WIN32 AND NOT CYGWIN)
ENDFOREACH(X $(harnesscode))

INSTALL_TARGETS(/lib ${name})
FILE(GLOB header_1 include/yarp/*.h)
INSTALL_FILES(/include/yarp FILES ${header_1})
FILE(GLOB header_2 include/yarp/${postfix}/*.h)
INSTALL_FILES(/include/yarp/${postfix} FILES ${header_2})

# pasa addition. still under development.
#IF (WIN32 AND NOT CYGWIN)
#  ADD_CUSTOM_COMMAND(TARGET ${name} POST_BUILD COMMAND xcopy ARGS include\\*.h ${CMAKE_INSTALL_PREFIX}\\include\\ /S /C /Y /I)
#ELSE (WIN32 AND NOT CYGWIN)
#
#ENDIF (WIN32 AND NOT CYGWIN)

ENDMACRO(YarpLibrary)

