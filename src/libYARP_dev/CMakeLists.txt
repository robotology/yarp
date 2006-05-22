INCLUDE(ListUtils.cmake)

##### LOCAL MACROS
MACRO(DD_FIND_LOCAL_LIB var dd LIBS)
  FOREACH(lib ${LIBS})
	SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${dd}/${OS_TAG}/dd_orig/lib)
	FILE(GLOB TEMP ${LOCAL_PATH}/${lib})
	IF (TEMP)
	  MESSAGE(STATUS "OK ${lib} found in ${LOCAL_PATH}")
	  SET(PATH-${DD} ${LOCAL_PATH}/${lib} CACHE FILEPATH "Lib required by ${DD}")
	ELSE(TEMP)
	  MESSAGE(ERROR "${lib} NOT found in ${LOCAL_PATH}")
	  SET(PATH-${DD}-${lib} "NOT-FOUND: ${lib}" CACHE FILEPATH "Path to ${lib} (required by ${DD})")
	ENDIF(TEMP)
  ENDFOREACH(lib ${ARGN})
ENDMACRO(DD_FIND_LOCAL_LIB)

MACRO(DD_FIND_EXTERNAL_LIB var dd LIBS)
  MESSAGE(STATUS "WARNING: ${dd} requires proper installation of: ${LIBS}, YARP assumes you took care of that.")
ENDMACRO(DD_FIND_EXTERNAL_LIB)
##### END  LOCAL MACROS

### BEGIN PROJECT CODE
PROJECT(libYARP_dev)

#the name of the project, the output will be this name .lib (in windows "d" will be added
#to this name to compile the debug version)
SET(PROJECT_NAME YARP_dev)
# set the OS_TAG variable to "linux" or "winnt"
YarpTag()

SET(AVAILABLE_DEVICES "") #default is an empty list

INCLUDE(AvailableDevices.txt)

# cycle all available devices
FOREACH(DD ${AVAILABLE_DEVICES})
  SET(FLAG-${DD} FALSE CACHE BOOL "Do you want to add ${DD} to libYARP_dev?")
ENDFOREACH(DD ${AVAILABLE_DEVICES})

# re-check all DD to find the ones that were selected by the user
FOREACH(DD ${AVAILABLE_DEVICES})
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
INCLUDE_DIRECTORIES(include ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
  ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${ACE_INCLUDE_DIR})

# parse all dd to collect source and header files, check dependencies
FOREACH(DD ${FOUND_DD})
  SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
#  MESSAGE(STATUS "Reading ${LOCAL_PATH}/libraries.txt:") 
  FILE(READ ${LOCAL_PATH}/libraries.txt FILE)
#  MESSAGE(STATUS "${FILE}")
  CAR(TOKEN ${FILE})
  CDR(LIB_LIST ${FILE})
  IF (TOKEN STREQUAL YARP_LOCAL)
#	MESSAGE(STATUS "${DD} is going to use local libs: ${LIB_LIST}")
	DD_FIND_LOCAL_LIB(DUMMY ${DD} "${LIB_LIST}")
  ELSE (TOKEN STREQUAL YARP_LOCAL)
	DD_FIND_EXTERNAL_LIB(DUMMY ${DD} "${LIB_LIST}")
  ENDIF(TOKEN STREQUAL YARP_LOCAL)

  MESSAGE(STATUS "Adding source files for ${DD}")
  AUX_SOURCE_DIRECTORY(src/${DD}/common libcode1)
  AUX_SOURCE_DIRECTORY(src/${DD}/${OS_TAG}/yarp libcode2)
  SET(libcode ${libcode1} ${libcode2})
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/common)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
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
# A FEW MESSAGES USEFUL FOR DEBUG
#MESSAGE(STATUS "Adding source files in:")
#MESSAGE(STATUS "${libcode}")
#MESSAGE(STATUS "Adding header files:")
#MESSAGE(STATUS "${libheader}")

#avoid empty projects...
IF(FOUND_ONE)
  ADD_LIBRARY(${PROJECT_NAME} ${libcode} ${libheader})
  ### INSTALL RULES ###
  #libYARP_dev.a or YARP_dev.lib will be copied in CMAKE_INSTALL_PREFIX/lib
  INSTALL_TARGETS(/lib YARP_dev) 
  FILE(GLOB installedHeaders include/yarp/dev/*.h) 
  #libYARP_dev/include/yarp/dev/*.h will be copied in CMAKE_INSTALL_PREFIX/include/yarp/dev
  INSTALL_FILES(/include/yarp/dev FILES ${installedHeaders})
ENDIF(FOUND_ONE)

