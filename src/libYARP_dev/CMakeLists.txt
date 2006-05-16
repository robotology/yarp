PROJECT(libYARP_dev)

#the name of the project, the output will be this name .lib (in windows "d" will be added
#to this name to compile the debug version
SET(PROJECT_NAME YARP_dev)


IF (WIN32 AND NOT CYGWIN)
	SET(avail_dd picolo dragonfly)
	SET(dd_libs Picolo32 PGRFlyCapture)
  	#search DD libs	
	FOREACH(DD ${dd_libs})
		SET(DD_PATH_VAR_NAME PATH-${DD})
		
		FOREACH(PATH ${avail_dd})
			SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${PATH}/${OS_TAG}/dd_orig/lib)
			FIND_LIBRARY(${DD_PATH_VAR_NAME} ${DD} ${LOCAL_PATH})
			ENDIF(DD_PATH_VAR_NAME)
		ENDFOREACH(PATH ${avail_DD})

		SET(DD_PATHS ${DD_PATHS} ${${DD_PATH_VAR_NAME}})
	ENDFOREACH(DD ${dd_libs})
ENDIF (WIN32 AND NOT CYGWIN)

IF (UNIX)
   SET(avail_dd dragonfly/linux)
ENDIF (UNIX)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include 
			${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
			${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${ACE_INCLUDE_DIR})

LINK_DIRECTORIES(${dd_dir_libs})

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code

# parse all dd and collect source and header files
FOREACH(X ${avail_dd})
	# source files, (AUX_SOURCE_DIRECTORY seems to append to the output list)
	AUX_SOURCE_DIRECTORY(src/${X}/${OS_TAG}/yarp libcode)
	AUX_SOURCE_DIRECTORY(src/${X}/common libcode)

	FILE(GLOB_RECURSE tmpSpecHeaders src/${X}/${OS_CODE}/yarp/*.h)
	FILE(GLOB_RECURSE tmpCommonHeaders src/${X}/common/*.h)
	
	SET(libheaders ${libheaders} ${tmpSpecHeaders})
	SET(libheaders ${libheaders} ${tmpCommonHeaders})
ENDFOREACH(X $(avail_dd))
FILE(GLOB_RECURSE libheader2 include/*.h)
SET(libheader ${libheaders} ${libheader2})
SOURCE_GROUP("Header Files" FILES ${libheader})

INCLUDE(YarpReqLib)

ADD_LIBRARY(${PROJECT_NAME} ${libcode} ${libheader})
INSTALL_TARGETS(/lib ${PROJECT_NAME})

#SET(postbuild1 lib ${LIBRARY_OUTPUT_PATH}/Debug/${PROJECT_NAME}d.lib ${DD_PATHS} 
#				/OUT:${LIBRARY_OUTPUT_PATH}/Debug/${PROJECT_NAME}d.lib)
#SET(postbuild2 lib ${LIBRARY_OUTPUT_PATH}/Release/${PROJECT_NAME}.lib ${DD_PATHS} 
#				/OUT:${LIBRARY_OUTPUT_PATH}/Release/${PROJECT_NAME}.lib)

#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${postbuild1} COMMENT "Post build: library creation")
#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${postbuild2} COMMENT "Post build: library creation")

#LINK_LIBRARIES(YARP_dev YARP_OS)
