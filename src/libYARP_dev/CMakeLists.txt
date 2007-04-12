INCLUDE(ListUtils.cmake)
INCLUDE(YarpAddExtModule)

SET(EXT_LIBS ${})
SET(ALL_SRC ${})

##### LOCAL MACROS
MACRO(DD_FIND_LOCAL_LIB var dd LIBS)
  FOREACH(lib ${LIBS})
	SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${dd}/${OS_TAG}/dd_orig/lib)
	FILE(GLOB TEMP ${LOCAL_PATH} ${lib})
	IF (TEMP)
	  MESSAGE(STATUS "OK ${lib} found in ${LOCAL_PATH}")
   	  SET(EXT_LIBS "${EXT_LIBS};${LOCAL_PATH}/${lib}")
	  SET(PATH-${DD} ${LOCAL_PATH}/${lib} CACHE FILEPATH "Lib required by ${DD}")
	ELSE(TEMP)
	  MESSAGE(ERROR "${lib} NOT found in ${LOCAL_PATH}")
	  SET(PATH-${DD}-${lib} "NOT-FOUND: ${lib}" CACHE FILEPATH "Path to ${lib} (required by ${DD})")
	ENDIF(TEMP)
  ENDFOREACH(lib ${ARGN})
ENDMACRO(DD_FIND_LOCAL_LIB)

MACRO(DD_FIND_EXTERNAL_LIB var dd LIBS)
  MESSAGE(STATUS "WARNING: ${dd} requires proper installation of: ${LIBS}, YARP assumes you took care of that.")
  SET(EXT_LIBS "${EXT_LIBS};${LIBS}")
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

IF (CREATE_DEVICE_LIBRARY_BUILTINS)
   INCLUDE(AvailableDevices.txt)
ELSE (CREATE_DEVICE_LIBRARY_BUILTINS)
     SET(AVAILABLE_DEVICES "")
ENDIF (CREATE_DEVICE_LIBRARY_BUILTINS)

# cycle all available devices -- creates a cache entry for each of them
# so the user can enable/disable them. Devices that are disabled are
# NOT compiled in libYARP_dev. ENABLE_${DD} is also used when CMake
# generates PolulateDriver.cpp, see below.
FOREACH(DD ${AVAILABLE_DEVICES})
  SET(ENABLE_${DD} FALSE CACHE BOOL "Do you want to add ${DD} to libYARP_dev?")
ENDFOREACH(DD ${AVAILABLE_DEVICES})

# cycle additional modules -- creates a cache entry for each of them.
# These modules at the moment are always compiled
# in libYARP_dev. ENABLE_${AM} (by default true) is used when CMake 
# generates PopulateDriver.cpp, see below.
FOREACH(AM ${ADDITIONAL_MODULES})
  SET(ENABLE_${AM} TRUE CACHE INTERNAL "Select additional module ${AM}?")
ENDFOREACH(AM ${ADDITIONAL_MODULES})

SET(populator ${CMAKE_BINARY_DIR}/src/libYARP_dev/src_generated/PopulateDrivers.cpp)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp.in
  ${populator} @ONLY  IMMEDIATE)

# any non-driver-specific source code
#AUX_SOURCE_DIRECTORY(src libcode_general)
FILE(GLOB libcode_general src/*.cpp)

# re-check all DD to find the ones that were selected by the user
FOREACH(DD ${AVAILABLE_DEVICES})
  IF (ENABLE_${DD})
    SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
    MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 
    FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
    IF(NOT TEMP_PATH)
        SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/default)
	MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 
	FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
    ENDIF(NOT TEMP_PATH)
    IF(TEMP_PATH)
	  MESSAGE(STATUS "Found libraries.txt for ${DD}") 
	  SET(FOUND_DD ${FOUND_DD} ${DD})
	  SET(FOUND_ONE TRUE) #at least one dd was found
    ELSE(TEMP_PATH)
	  MESSAGE(STATUS "No libraries.txt found for ${DD}, skipping it")
    ENDIF(TEMP_PATH)
  ENDIF (ENABLE_${DD})
ENDFOREACH(DD ${avail_dd})

## COMMON INCLUDE FILES
INCLUDE_DIRECTORIES(include ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
  ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${CMAKE_SOURCE_DIR}/src/libYARP_sig/include ${ACE_INCLUDE_DIR})
SET(header_path ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include)

# parse all dd to collect source and header files, check dependencies
FOREACH(DD ${FOUND_DD})
  SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${OS_TAG})
  SET(LOCAL_TAG ${OS_TAG})
  FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
  IF(NOT TEMP_PATH)
        SET(LOCAL_PATH ${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/default)
	SET(LOCAL_TAG default)
	MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 
	FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
  ENDIF(NOT TEMP_PATH)
  #  MESSAGE(STATUS "Reading ${LOCAL_PATH}/libraries.txt:") 
  FILE(READ ${LOCAL_PATH}/libraries.txt FILE)
  #  MESSAGE(STATUS "${FILE}")
  CAR(TOKEN ${FILE})
  CDR(LIB_LIST ${FILE})
  IF (TOKEN STREQUAL YARP_LOCAL)
	#	MESSAGE(STATUS "${DD} is going to use local libs: ${LIB_LIST}")
	DD_FIND_LOCAL_LIB(DUMMY ${DD} "${LIB_LIST}")
  ENDIF (TOKEN STREQUAL YARP_LOCAL)
  IF (TOKEN STREQUAL YARP_EXTERNAL)
	DD_FIND_EXTERNAL_LIB(DUMMY ${DD} "${LIB_LIST}")
  ENDIF(TOKEN STREQUAL YARP_EXTERNAL)
  IF (TOKEN STREQUAL YARP_CMAKE)
	    # currently works with just ONE library
	    CAR(libname ${LIB_LIST})
	    MESSAGE(STATUS "Checking for library dependency " ${libname})
	FIND_PACKAGE("${libname}" REQUIRED)
	STRING(TOUPPER "${libname}" uplibname)
	INCLUDE_DIRECTORIES(${${uplibname}_INCLUDE_DIR})
	LINK_DIRECTORIES(${${uplibname}_LINK_DIRECTORIES})
        SET(EXT_LIBS "${EXT_LIBS};${${uplibname}_LIBRARIES}")
	LINK_LIBRARIES(${${uplibname}_LIBRARIES})
  ENDIF(TOKEN STREQUAL YARP_CMAKE)

  FILE(GLOB libcode1 src/${DD}/common/yarp/*.cpp)
  #AUX_SOURCE_DIRECTORY(src/${DD}/common libcode1)
  #MESSAGE(STATUS "Source code for ${DD} is ${libcode1}")
  ## this message never works
  FILE(GLOB libcode2 src/${DD}/${LOCAL_TAG}/yarp/*.cpp)
  #AUX_SOURCE_DIRECTORY(src/${DD}/${OS_TAG}/yarp libcode2)
  SET(libcode ${libcode1} ${libcode2})
  SET(ALL_SRC ${ALL_SRC} ${libcode})
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/common)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/${LOCAL_TAG})
  SET(header_path "${header_path};${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/${DD}/common")
  # header files
  FILE(GLOB_RECURSE tmpHeaders src/${DD}/${LOCAL_TAG}/yarp/*.h)
  SET(libheaders ${libheaders} ${tmpHeaders})
  FILE(GLOB_RECURSE tmpHeaders src/${DD}/common/*.h)
  SET(libheaders ${libheaders} ${tmpHeaders})
ENDFOREACH(DD ${FOUND_DD})

###### 
# This code is part of the new method to add external devices 
# at this time this is still experimental. It should replace
# the code above.
SET(SAVED_TMP "${PROJECT_NAME}")

SET(YARP_MODULE_PATH "${CMAKE_SOURCE_DIR}/conf")
SET(EXTERNAL_MODULES_FILE "${YARP_MODULE_PATH}/ExternalModules.cmake")

IF (EXISTS ${EXTERNAL_MODULES_FILE})
  INCLUDE(${EXTERNAL_MODULES_FILE})

  ## parse the list of modules
  FOREACH(MOD ${EXTERNAL_MODULES})
	SET(MOD_PATH ${${MOD}_PATH})
	MESSAGE(STATUS "Adding module ${MOD} from ${MOD_PATH}")

	# search each module for devices
	YarpAddExtModule(${MOD} ${MOD_PATH})
	
	# populate list of header folders
	SET(ext_lib_headers "${ext_lib_headers}" "${CMAKE_BINARY_DIR}/generated_code/${MOD}")
	  
	# populate list of libraries
	IF (WIN32 AND NOT CYGWIN)
	  SET(libname "${MOD}.lib")
	  STRING(REPLACE ".lib" "d.lib" libname2 ${libname})
	  SET(EXT_LIBS "${EXT_LIBS}" "optimized;${libname};debug;${libname2}")
	ELSE(WIN32 AND NOT CYGWIN)
	  # populate list of dependencies
	  SET(EXT_LIBS "${EXT_LIBS}" "${MOD}")
	ENDIF (WIN32 AND NOT CYGWIN)
  ENDFOREACH(MOD ${EXTERNAL_MODULES})
ENDIF(EXISTS ${EXTERNAL_MODULES_FILE})

### patch PopulateDrivers.cpp
WRITE_FILE(${populator} "//Automatically generated by CMAKE, external modules" APPEND)
IF (EXTERNAL_MODULES)
  FOREACH(MOD ${EXTERNAL_MODULES})
	WRITE_FILE(${populator} "#include \"${MOD}Adder.h\"" APPEND)	
  ENDFOREACH(MOD ${EXTERNAL_MODULES})

  WRITE_FILE(${populator} "void addExternalDevices()\n{" APPEND)

  FOREACH(MOD ${EXTERNAL_MODULES})
	WRITE_FILE(${populator} "    add${MOD}();" APPEND)
  ENDFOREACH(MOD ${EXTERNAL_MODULES})

  WRITE_FILE(${populator} "}\n" APPEND)
ELSE(EXTERNAL_MODULES)
  WRITE_FILE(${populator} "void addExternalDevices()" APPEND)
  WRITE_FILE(${populator} "{\n//do nothing\n}\n" APPEND)
ENDIF(EXTERNAL_MODULES)

SET(PROJECT_NAME "${SAVED_TMP}")
################ 

##### add include files to project (visual studio)
# this only works if the files are also included in the project, see
# ADD_LIBRARY below
FILE(GLOB_RECURSE libheader2 include/*.h)
FILE(GLOB inlinefiles src/*.inl)
SET(libheader ${libheaders} ${libheader2})
SOURCE_GROUP("Header Files" FILES ${libheader})
SOURCE_GROUP("Inline Files" FILES ${inlinefiles})

SET(ALL_SRC ${ALL_SRC} ${libcode_general} ${populator})

# device library contains useful code
SET(FOUND_ONE TRUE)

#avoid empty projects...
IF(FOUND_ONE)
  # cmake does not like .inl files in linux/cygwin, but it is nice 
  # to have them listed in  the visual studio project.
  IF(WIN32 AND NOT CYGWIN)
	ADD_LIBRARY(${PROJECT_NAME} ${ALL_SRC} ${libheader} ${inlinefiles})
  ELSE(WIN32 AND NOT CYGWIN)
	ADD_LIBRARY(${PROJECT_NAME} ${ALL_SRC} ${libheader})
  ENDIF(WIN32 AND NOT CYGWIN)

  IF (ext_lib_headers)
	INCLUDE_DIRECTORIES(${ext_lib_headers})
  ENDIF(ext_lib_headers)

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

GET_TARGET_PROPERTY(YARP_dev_LIB ${PROJECT_NAME} LOCATION)
GET_TARGET_PROPERTY(YARP_dev_INC ${PROJECT_NAME} header_path)
GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS ${PROJECT_NAME} ext_libs)

IF (WIN32 AND NOT CYGWIN)
  SET(libname "${YARP_dev_LIB}")
  STRING(REPLACE ".lib" "d.lib" libname2 ${libname})
  SET(YARP_dev_LIB "optimized;${libname};debug;${libname2}" CACHE INTERNAL "libraries")
ENDIF (WIN32 AND NOT CYGWIN)
SET(YARP_dev_LIB ${YARP_dev_LIB} ${YARP_dev_EXT_LIBS})
IF (NOT YARP_dev_LIB)
  SET(YARP_dev_LIB "")
ENDIF (NOT YARP_dev_LIB)
IF (NOT YARP_dev_INC)
  SET(YARP_dev_INC "")
ENDIF (NOT YARP_dev_INC)

SET(YARP_dev_LIB "${YARP_dev_LIB}" CACHE INTERNAL "libraries")
SET(YARP_dev_INC "${YARP_dev_INC}" CACHE INTERNAL "include path")
SET(YARP_dev_EXT_LIBS "${YARP_dev_EXT_LIBS}" CACHE INTERNAL "extra libs")

# tests for devices
SET(postfix dev)
AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_${postfix} ${harnesscode})
TARGET_LINK_LIBRARIES(harness_${postfix} YARP_dev YARP_sig YARP_OS ${YARP_dev_EXT_LIBS} ${ACE_LINK_FLAGS})
GET_TARGET_PROPERTY(EXEC harness_${postfix} LOCATION)

# add in standard tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
IF(WIN32 AND NOT CYGWIN)
	  ADD_TEST(YARP_dev::${XN} ${EXECUTABLE_OUTPUT_PATH}/harness_${postfix} regression ${XN})
ELSE(WIN32 AND NOT CYGWIN)
	  ADD_TEST(YARP_dev::${XN} ${EXEC} regression ${XN})
ENDIF(WIN32 AND NOT CYGWIN)
ENDFOREACH(X $(harnesscode))

# add in hardware specific tests, if requested
IF (CREATE_BUILTIN_DEVICE_TESTS)
   # add in our tests
   FILE(GLOB inis harness/*.ini)
   FOREACH(X ${inis})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  GET_FILENAME_COMPONENT(XINI ${X} ABSOLUTE)
	  SET(XDOX ${CMAKE_BINARY_DIR}/src/libYARP_dev/harness/${XN}.dox)
	  ADD_TEST(device::${XN} ${EXECUTABLE_OUTPUT_PATH}/harness_${postfix} --file "${XINI}" --doc "${XDOX}")
   ENDFOREACH(X ${inis})
ENDIF (CREATE_BUILTIN_DEVICE_TESTS)

