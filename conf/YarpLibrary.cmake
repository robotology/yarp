
MACRO(YarpLibrary name postfix)

PROJECT(lib${name})

FOREACH(X ${NEED_LIBS})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib${X}/include)
ENDFOREACH(X ${NEED_LIBS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib${name}/include)

#AUX_SOURCE_DIRECTORY(src libcode)
FILE(GLOB_RECURSE folder_source src/*.cpp)
SET(libcode ${folder_source})
FILE(GLOB_RECURSE folder_header include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

INCLUDE(YarpReqLib)

ADD_LIBRARY(${name} ${libcode} ${folder_header})

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_${postfix} ${harnesscode})

TARGET_LINK_LIBRARIES(harness_${postfix} ${name} ${NEED_LIBS} ${ACE_LINK_FLAGS})

GET_TARGET_PROPERTY(EXEC harness_${postfix} LOCATION)

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(${name}::${XN} ${EXEC} regression ${XN})
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

