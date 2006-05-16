PROJECT(libYARP_dev)

#the name of the project, the output will be this name .lib (in windows "d" will be added
#to this name to compile the debug version
SET(PROJECT_NAME YARP_dev)

IF (WIN32 AND NOT CYGWIN)
	# list of available device drivers
	SET(avail_dd picolo dragonfly)
	# list of dd libraries
#	SET(dd_libs Picolo32 PGRFlyCapture)
   SET(OS_LOCAL_FOLDER winnt)
ENDIF (WIN32 AND NOT CYGWIN)

IF (UNIX)
   SET(avail_dd dragonfly)
#   SET(dd_libs libraw1394)
   SET(OS_LOCAL_FOLDER linux)
ENDIF (UNIX)

#search DD libs	
SET(DD_PATH_VAR_NAME PATH$-{DD})
		
FOREACH(DD ${avail_dd})
		SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_LOCAL_FOLDER})
		MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 

		FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
		IF(TEMP_PATH)
				MESSAGE(STATUS "FOUND libraries.txt for ${DD}") 
				SET(FOUND_DD ${FOUND_DD} ${DD})
		ELSE(TEMP_PATH)
				MESSAGE(STATUS "No libraries.txt found for ${DD} skipping")
		ENDIF(TEMP_PATH)

ENDFOREACH(DD ${avail_DD})

## COMMON INCLUDE FILES
INCLUDE_DIRECTORIES(include ../libYARP_OS/include 
			${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
			${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${ACE_INCLUDE_DIR})


# parse all dd and collect source and header files
FOREACH(DD ${FOUND_DD})

			 # source files, (AUX_SOURCE_DIRECTORY seems to append to the output list)
			 AUX_SOURCE_DIRECTORY(src/${DD}/common libcode)
			 AUX_SOURCE_DIRECTORY(src/${DD}/${OS_LOCAL_FOLDER}/yarp libcode)

			 # header files
			 FILE(GLOB_RECURSE tmpHeaders src/${DD}/${OS_LOCAL_FOLDER}/yarp/*.h)
			 SET(libheaders ${libheaders} ${tmpHeaders})
ENDFOREACH(DD $(FOUND_DD))

#add include files to project (visual studio)
FILE(GLOB_RECURSE libheader2 include/*.h)
SET(libheader ${libheaders} ${libheader2})
SOURCE_GROUP("Header Files" FILES ${libheader})

INCLUDE(YarpReqLib)

ADD_LIBRARY(${PROJECT_NAME} ${libcode} ${libheader})
INSTALL_TARGETS(/lib ${PROJECT_NAME})