INCLUDE(ListUtils.cmake)

PROJECT(libYARP_dev)

#the name of the project, the output will be this name .lib (in windows "d" will be added
#to this name to compile the debug version
SET(PROJECT_NAME YARP_dev)
# set the OS_TAG variable to "linux" or "winnt"
YarpTag()

IF (WIN32 AND NOT CYGWIN)
	# list of available device drivers
	SET(avail_dd picolo dragonfly)
ENDIF (WIN32 AND NOT CYGWIN)

IF (UNIX)
   SET(avail_dd dragonfly)
ENDIF (UNIX)

FOREACH(DD ${avail_dd})
        SET(FLAG-${DD} FALSE CACHE BOOL "Do you want to compile ${DD}?")
ENDFOREACH(DD ${avail_DD})

# re-check all DD to see which ones were selected
FOREACH(DD ${avail_dd})
   IF (FLAG-${DD})
        SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
        MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 
        FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
        IF(TEMP_PATH)
		MESSAGE(STATUS "Found libraries.txt for ${DD}") 
		SET(FOUND_DD ${FOUND_DD} ${DD})
		SET(FOUND_ONE TRUE) #at least one dd was found
        ELSE(TEMP_PATH)
		MESSAGE(STATUS "No libraries.txt found for ${DD}, skipping it")
        ENDIF(TEMP_PATH)
    ENDIF (FLAG-${DD})
ENDFOREACH(DD ${avail_dd})

## COMMON INCLUDE FILES
INCLUDE_DIRECTORIES(include ../libYARP_OS/include 
			    ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
			    ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${ACE_INCLUDE_DIR})

# parse all dd and collect source and header files
FOREACH(DD ${FOUND_DD})
        SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
        MESSAGE(STATUS "Reading ${LOCAL_PATH}/libraries.txt:") 
        FILE(READ ${LOCAL_PATH}/libraries.txt FILE)
	MESSAGE("${FILE}")
	CAR(TOKEN ${FILE})
	CDR(LIB_LIST ${FILE})
	IF (TOKEN STREQUAL YARP_LOCAL)
	   MESSAGE(STATUS "${DD} is going to use local libs: ${LIB_LIST}")
	   DD_FIND_LOCAL_LIB(DUMMY ${DD} "${LIB_LIST}")
	ELSE (TOKEN STREQUAL YARP_LOCAL)
	   DD_FIND_EXTERNAL_LIB(DUMMY ${DD} "${LIB_LIST}")
	ENDIF(TOKEN STREQUAL YARP_LOCAL)

	MESSAGE(STATUS "Adding source for ${DD}")
	 AUX_SOURCE_DIRECTORY(src/${DD}/common libcode1)
	 AUX_SOURCE_DIRECTORY(src/${DD}/${OS_TAG}/yarp libcode2)
	 SET(libcode ${libcode1} ${libcode2})
	 INCLUDE_DIRECTORIES(src/${DD}/common)
	 INCLUDE_DIRECTORIES(src/${DD}/${OS_TAG}/yarp)
	 # header files
	 FILE(GLOB_RECURSE tmpHeaders src/${DD}/${OS_TAG}/yarp/*.h)
	 SET(libheaders ${libheaders} ${tmpHeaders})
	 FILE(GLOB_RECURSE tmpHeaders src/${DD}/common/*.h)
 SET(libheaders ${libheaders} ${tmpHeaders})
ENDFOREACH(DD ${FOUND_DD})

#add include files to project (visual studio)
FILE(GLOB_RECURSE libheader2 include/*.h)
SET(libheader ${libheaders} ${libheader2})
SOURCE_GROUP("Header Files" FILES ${libheader})

INCLUDE(YarpReqLib)
MESSAGE(STATUS "Adding source files in:")
MESSAGE("${libcode}")
MESSAGE(STATUS "Adding header files:")
MESSAGE("${libheader}")

#avoid empty projects...
IF(FOUND_ONE)
	ADD_LIBRARY(${PROJECT_NAME} ${libcode} ${libheader})
ENDIF(FOUND_ONE)
