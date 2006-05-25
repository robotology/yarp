INCLUDE(ListUtils.cmake)

SET(EXT_LIBS "")
SET(ALL_SRC "")

##### LOCAL MACROS
MACRO(DD_FIND_LOCAL_LIB var dd LIBS)
  FOREACH(lib ${LIBS})
	SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${dd}/${OS_TAG}/dd_orig/lib)
	FILE(GLOB TEMP ${LOCAL_PATH}/${lib})
	IF (TEMP)
	  MESSAGE(STATUS "OK ${lib} found in ${LOCAL_PATH}")
   	  SET(EXT_LIBS "${EXT_LIBS} ${LOCAL_PATH}/${lib}")
	  SET(PATH-${DD} ${LOCAL_PATH}/${lib} CACHE FILEPATH "Lib required by ${DD}")
	ELSE(TEMP)
	  MESSAGE(ERROR "${lib} NOT found in ${LOCAL_PATH}")
	  SET(PATH-${DD}-${lib} "NOT-FOUND: ${lib}" CACHE FILEPATH "Path to ${lib} (required by ${DD})")
	ENDIF(TEMP)
  ENDFOREACH(lib ${ARGN})
ENDMACRO(DD_FIND_LOCAL_LIB)

MACRO(DD_FIND_EXTERNAL_LIB var dd LIBS)
  MESSAGE(STATUS "WARNING: ${dd} requires proper installation of: ${LIBS}, YARP assumes you took care of that.")
  SET(EXT_LIBS "${EXT_LIBS} ${LIBS}")
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
  # cannot use - symbol when configuring external file
  SET(ENABLE_${DD} "${FLAG-${DD}}" CACHE INTERNAL "config driver")
ENDFOREACH(DD ${AVAILABLE_DEVICES})

# any non-driver-specific source code
#AUX_SOURCE_DIRECTORY(src libcode_general)
FILE(GLOB libcode_general src/*.cpp)

SET(populator ${CMAKE_BINARY_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp.in
               ${CMAKE_BINARY_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp @ONLY  IMMEDIATE)


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
SET(header_path ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include)


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
  FILE(GLOB libcode1 src/${DD}/common/*.cpp)
  #AUX_SOURCE_DIRECTORY(src/${DD}/common libcode1)
  FILE(GLOB libcode2 src/${DD}/${OS_TAG}/yarp/*.cpp)
  #AUX_SOURCE_DIRECTORY(src/${DD}/${OS_TAG}/yarp libcode2)
  SET(libcode ${libcode1} ${libcode2})
  SET(ALL_SRC ${ALL_SRC} ${libcode})
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/common)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
  SET(header_path "${header_path} ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/common")
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


SET(ALL_SRC ${ALL_SRC} ${libcode_general} ${populator})


#avoid empty projects...
IF(FOUND_ONE)
  ADD_LIBRARY(${PROJECT_NAME} ${ALL_SRC} ${libheader})
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES header_path "${header_path}")
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES ext_libs "${EXT_LIBS}")
  ### INSTALL RULES ###
  #libYARP_dev.a or YARP_dev.lib will be copied in CMAKE_INSTALL_PREFIX/lib
  INSTALL_TARGETS(/lib YARP_dev) 
  FILE(GLOB installedHeaders include/yarp/dev/*.h) 
  #libYARP_dev/include/yarp/dev/*.h will be copied in CMAKE_INSTALL_PREFIX/include/yarp/dev
  INSTALL_FILES(/include/yarp/dev FILES ${installedHeaders})
ENDIF(FOUND_ONE)


# Prepare properties for external users of library

GET_TARGET_PROPERTY(YARP_dev_LIB YARP_dev LOCATION)
GET_TARGET_PROPERTY(YARP_dev_INC YARP_dev header_path)
GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS YARP_dev ext_libs)
IF (WIN32 AND NOT CYGWIN)
   SET(libname "${YARP_dev_LIB}")
   STRING(REPLACE ".lib" "d.lib" libname2 ${libname})
   SET(YARP_dev_LIB "optimized ${libname} debug ${libname2}" CACHE INTERNAL "libraries")
ENDIF (WIN32 AND NOT CYGWIN)
SET(YARP_dev_LIB "${YARP_dev_LIB} ${YARP_dev_EXT_LIBS}")
IF (NOT YARP_dev_LIB)
   SET(YARP_dev_LIB "")
ENDIF (NOT YARP_dev_LIB)
IF (NOT YARP_dev_INC)
   SET(YARP_dev_INC "")
ENDIF (NOT YARP_dev_INC)

SET(YARP_dev_LIB "${YARP_dev_LIB}" CACHE INTERNAL "libraries")
SET(YARP_dev_INC "${YARP_dev_INC}" CACHE INTERNAL "include path")
SET(YARP_dev_EXT_LIBS "${YARP_dev_EXT_LIBS}" CACHE INTERNAL "extra libs")

