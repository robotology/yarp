# clean variables
SET(YARP_EXTMOD_TARGETS "")
SET(YARP_EXTMOD_LIBS "")

IF (NOT COMPILING_ALL_YARP)
   PROJECT(FloatingYarpDev)
   # YARP_DIR should be set
   IF (NOT YARP_DIR)
     SET (YARP_DIR "$ENV{YARP_DIR}")
   ENDIF (NOT YARP_DIR)
   FIND_PACKAGE(YARP REQUIRED)

   # MESSAGE(STATUS "YARP_DIR" ${YARP_DIR})
   # Pick up our scripts - they are all in the conf subdirectory
   SET(CMAKE_MODULE_PATH ${YARP_DIR}/conf)

   INCLUDE(YarpTag)
   #INCLUDE(YarpOptions)
   #INCLUDE(YarpReqLib)
   INCLUDE(UsePkgConfig)
   SET(BASE_BINARY_DIR ${CMAKE_BINARY_DIR})
   SET(BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
#   INCLUDE_DIRECTORIES(${BASE_SOURCE_DIR}/src/libYARP_OS/include)
#   INCLUDE_DIRECTORIES(${BASE_SOURCE_DIR}/src/libYARP_sig/include)
#   INCLUDE_DIRECTORIES(${BASE_SOURCE_DIR}/src/libYARP_dev/include)
   SET(REL_DIR "/") 
   SET(REL_SRC "")
   SET(YARP_MODULE_PATH "${YARP_DIR}/conf")
   SET(DISTINCT "Very")
ELSE (NOT COMPILING_ALL_YARP)
   SET(BASE_BINARY_DIR ${CMAKE_BINARY_DIR})
   SET(BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
   SET(REL_DIR "/src/libYARP_dev/")
   SET(REL_SRC "src/")
   SET(YARP_MODULE_PATH "${BASE_SOURCE_DIR}/conf")
   SET(DISTINCT "")
ENDIF (NOT COMPILING_ALL_YARP)

INCLUDE(YarpListUtils)
INCLUDE(YarpAddExtModule)

SET(EXT_LIBS ${})
SET(ALL_SRC ${})

##### LOCAL MACROS
MACRO(DD_FIND_LOCAL_LIB var dd LIBS)
  FOREACH(lib ${LIBS})
	SET(LOCAL_PATH ${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${dd}/${OS_TAG}/dd_orig/lib)
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

IF (COMPILING_ALL_YARP)
  SET(PROJECT_NAME YARP_dev)
ELSE (COMPILING_ALL_YARP)
  SET(PROJECT_NAME runnable)
ENDIF (COMPILING_ALL_YARP)

#the name of the project, the output will be this name .lib (in windows "d" will be added
#to this name to compile the debug version)
# set the OS_TAG variable to "linux" or "winnt"
YarpTag()

SET(AVAILABLE_DEVICES "") #default is an empty list

SET(AVAILABLE_DEVICES_FILE "${BASE_SOURCE_DIR}${REL_DIR}AvailableDevices.txt")

#MESSAGE(STATUS "checking ${AVAILABLE_DEVICES_FILE}")
IF (EXISTS ${AVAILABLE_DEVICES_FILE})
  IF (CREATE_DEVICE_LIBRARY_BUILTINS)
    INCLUDE(${AVAILABLE_DEVICES_FILE})
  ELSE (CREATE_DEVICE_LIBRARY_BUILTINS)
    SET(AVAILABLE_DEVICES "")
  ENDIF (CREATE_DEVICE_LIBRARY_BUILTINS)
ENDIF (EXISTS ${AVAILABLE_DEVICES_FILE})

# cycle all available devices -- creates a cache entry for each of them
# so the user can enable/disable them. Devices that are disabled are
# NOT compiled in libYARP_dev. ENABLE_${DD} is also used when CMake
# generates PopulateDriver.cpp, see below.
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

# any non-driver-specific source code
#AUX_SOURCE_DIRECTORY(src libcode_general)
FILE(GLOB libcode_general ${REL_SRC}*.cpp)

# re-check all DD to find the ones that were selected by the user
FOREACH(DD ${AVAILABLE_DEVICES})
  IF (ENABLE_${DD})
    SET(LOCAL_PATH ${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/${OS_TAG})
    MESSAGE(STATUS "Parsing ${LOCAL_PATH}") 
    FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
    IF(NOT TEMP_PATH)
      SET(LOCAL_PATH ${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/default)
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
INCLUDE_DIRECTORIES(include ${BASE_SOURCE_DIR}/src/libYARP_dev/include 
  ${BASE_SOURCE_DIR}/src/libYARP_OS/include ${BASE_SOURCE_DIR}/src/libYARP_sig/include ${ACE_INCLUDE_DIR})
SET(header_path ${BASE_SOURCE_DIR}/src/libYARP_dev/include)

# parse all dd to collect source and header files, check dependencies
FOREACH(DD ${FOUND_DD})
  SET(LOCAL_PATH ${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/${OS_TAG})
  SET(LOCAL_TAG ${OS_TAG})
  FILE(GLOB TEMP_PATH ${LOCAL_PATH}/libraries.txt)
  IF(NOT TEMP_PATH)
    SET(LOCAL_PATH ${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/default)
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
	#LINK_LIBRARIES(${${uplibname}_LIBRARIES})
  ENDIF(TOKEN STREQUAL YARP_CMAKE)

  FILE(GLOB libcode1 ${REL_SRC}${DD}/common/yarp/*.cpp)
  #AUX_SOURCE_DIRECTORY(${REL_SRC}${DD}/common libcode1)
  #MESSAGE(STATUS "Source code for ${DD} is ${libcode1}")
  ## this message never works
  FILE(GLOB libcode2 ${REL_SRC}${DD}/${LOCAL_TAG}/yarp/*.cpp)
  #AUX_SOURCE_DIRECTORY(${REL_SRC}${DD}/${OS_TAG}/yarp libcode2)
  SET(libcode ${libcode1} ${libcode2})
  SET(ALL_SRC ${ALL_SRC} ${libcode})
  INCLUDE_DIRECTORIES(${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/common)
  INCLUDE_DIRECTORIES(${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/${LOCAL_TAG})
  SET(header_path "${header_path};${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}${DD}/common")
  # header files
  FILE(GLOB_RECURSE tmpHeaders ${REL_SRC}${DD}/${LOCAL_TAG}/yarp/*.h)
  SET(libheaders ${libheaders} ${tmpHeaders})
  FILE(GLOB_RECURSE tmpHeaders ${REL_SRC}${DD}/common/*.h)
  SET(libheaders ${libheaders} ${tmpHeaders})
ENDFOREACH(DD ${FOUND_DD})

###### 
# This code is part of the new method to add external devices 
# at this time this is still experimental. It should replace
# the code above.
SET(SAVED_TMP "${PROJECT_NAME}")

SET(EXTERNAL_MODULES_FILE "${YARP_MODULE_PATH}/ExternalModules.cmake")

IF (NOT COMPILING_ALL_YARP)
  SET(EXTERNAL_MODULES localmod)
  SET(localmod_PATH ${BASE_SOURCE_DIR})
ELSE (NOT COMPILING_ALL_YARP)
  IF (EXISTS ${EXTERNAL_MODULES_FILE})
    INCLUDE(${EXTERNAL_MODULES_FILE})
  ENDIF (EXISTS ${EXTERNAL_MODULES_FILE})
ENDIF (NOT COMPILING_ALL_YARP)

IF (CREATE_DEVICE_LIBRARY_MODULES OR NEW_METHOD)
  # inhibit old compilation method
  SET (EXTERNAL_MODULES)
  SET (HACK_POPULATE_DRIVERS FALSE)
ELSE (CREATE_DEVICE_LIBRARY_MODULES OR NEW_METHOD)
  SET (HACK_POPULATE_DRIVERS TRUE)
  SET (EMBED_DEVICE_LIBRARY_CALL TRUE)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES OR NEW_METHOD)


IF (CREATE_DEVICE_LIBRARY_BUILTINS)
  SET (EMBED_DEVICE_LIBRARY_CALL TRUE)
ENDIF (CREATE_DEVICE_LIBRARY_BUILTINS)

SET(populator ${BASE_BINARY_DIR}${REL_DIR}src_generated/PopulateDrivers.cpp)

IF (COMPILING_ALL_YARP)
  CONFIGURE_FILE(${BASE_SOURCE_DIR}${REL_DIR}${REL_SRC}PopulateDrivers.cpp.in
    ${populator} @ONLY  IMMEDIATE)
ELSE (COMPILING_ALL_YARP)
  WRITE_FILE(${populator} "")
ENDIF (COMPILING_ALL_YARP)

IF (EXTERNAL_MODULES)
  ## parse the list of modules
  FOREACH(MOD ${EXTERNAL_MODULES})
	SET(MOD_PATH ${${MOD}_PATH})

	IF (EXISTS ${MOD_PATH})
	  MESSAGE(STATUS "Adding module ${MOD} from ${MOD_PATH}")

	  # search each module for devices
	  YarpAddExtModule(${MOD} ${MOD_PATH})
	  
	  # populate list of header folders
	  SET(ext_lib_headers "${ext_lib_headers}" "${BASE_BINARY_DIR}/generated_code/${MOD}")
	  
        # populate list of projects/targets to be added as "internal" dependencies for 
        # yarp built-in binaries
	  SET(YARP_EXTMOD_TARGETS "${YARP_EXTMOD_TARGETS}" ${MOD})
	  # populate list of libraries, for external programs using yarp
	  IF (WIN32 AND NOT CYGWIN)
		SET(libname "${MOD}.lib")
		STRING(REPLACE ".lib" "d.lib" libname2 ${libname})
		SET(YARP_EXTMOD_LIBS "${YARP_EXTMOD_LIBS}" "optimized;${libname};debug;${libname2}")
	  ELSE(WIN32 AND NOT CYGWIN)
		# populate list of dependencies
		SET(YARP_EXTMOD_LIBS "${YARP_EXTMOD_LIBS}" "${MOD}")
	  ENDIF (WIN32 AND NOT CYGWIN)

	  ## NOW BUILD DEPENDENCIES
	  SET(EXT_LIBS "${EXT_LIBS}" ${met_dependencies})
	  IF (unmet_dependencies)
		MESSAGE("Warning:")
		MESSAGE("List of lib/packages that are required by yarp but could not be found:")
		FOREACH(dep ${unmet_dependencies})
		  MESSAGE("${dep}")
		ENDFOREACH(dep ${unmet_dependencies})
		MESSAGE("")
	  ENDIF (unmet_dependencies)
	  
	ELSE (EXISTS ${MOD_PATH})
	  MESSAGE(SEND_ERROR "${MOD}_PATH is missing or it does not point to a valid path. Check ${EXTERNAL_MODULES_FILE}")
	  MESSAGE(STATUS "Skipping ${MOD}")
	ENDIF(EXISTS ${MOD_PATH})
	
  ENDFOREACH(MOD ${EXTERNAL_MODULES})
ENDIF(EXTERNAL_MODULES)

### patch PopulateDrivers.cpp
#### Changed this to run only if needed - it is order sensitive, and use 
#### of WRITE_FILE leads to unneeded compiles --paulfitz
IF (HACK_POPULATE_DRIVERS)
WRITE_FILE(${populator} "//Automatically generated by CMAKE, external modules" APPEND)
IF (EXTERNAL_MODULES)
  FOREACH(MOD ${EXTERNAL_MODULES})
	WRITE_FILE(${populator} "#include \"${MOD}Adder.h\"" APPEND)	
  ENDFOREACH(MOD ${EXTERNAL_MODULES})

  WRITE_FILE(${populator} "void add${DISTINCT}ExternalDevices()\n{" APPEND)

  FOREACH(MOD ${EXTERNAL_MODULES})
	WRITE_FILE(${populator} "    add${MOD}();" APPEND)
  ENDFOREACH(MOD ${EXTERNAL_MODULES})

  WRITE_FILE(${populator} "}\n" APPEND)
ELSE(EXTERNAL_MODULES)
  WRITE_FILE(${populator} "void add${DISTINCT}ExternalDevices()" APPEND)
  WRITE_FILE(${populator} "{\n//do nothing\n}\n" APPEND)
ENDIF(EXTERNAL_MODULES)
ENDIF (HACK_POPULATE_DRIVERS)

SET(PROJECT_NAME "${SAVED_TMP}")
################ 

##### add include files to project (visual studio)
# this only works if the files are also included in the project, see
# ADD_LIBRARY below
FILE(GLOB_RECURSE libheader2 include/*.h)
FILE(GLOB inlinefiles ${REL_SRC}*.inl)
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
  IF (COMPILING_ALL_YARP)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${EXT_LIBS})
  ENDIF (COMPILING_ALL_YARP)

  IF (ext_lib_headers)
	INCLUDE_DIRECTORIES(${ext_lib_headers})
  ENDIF(ext_lib_headers)

  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES header_path "${header_path}")
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES ext_libs "${EXT_LIBS}")
  ### INSTALL RULES ###
  #libYARP_dev.a or YARP_dev.lib will be copied in CMAKE_INSTALL_PREFIX/lib
  INSTALL_TARGETS(/lib ${PROJECT_NAME}) 
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
#SET(YARP_dev_LIB ${YARP_dev_LIB} ${YARP_dev_EXT_LIBS})
IF (NOT YARP_dev_LIB)
  SET(YARP_dev_LIB "")
ENDIF (NOT YARP_dev_LIB)
IF (NOT YARP_dev_INC)
  SET(YARP_dev_INC "")
ENDIF (NOT YARP_dev_INC)

SET(YARP_dev_LIB "${YARP_dev_LIB}" CACHE INTERNAL "libraries")
SET(YARP_dev_INC "${YARP_dev_INC}" CACHE INTERNAL "include path")
SET(YARP_dev_EXT_LIBS "${YARP_dev_EXT_LIBS}" CACHE INTERNAL "extra libs")

SET(YARP_EXTMOD_TARGETS "${YARP_EXTMOD_TARGETS}" CACHE INTERNAL "external modules targets")
SET(YARP_EXTMOD_LIBS "${YARP_EXTMOD_LIBS}" CACHE INTERNAL "external modules libraries")


IF (COMPILING_ALL_YARP)

   # tests for devices
   SET(postfix dev)
   AUX_SOURCE_DIRECTORY(harness harnesscode)
   ADD_EXECUTABLE(harness_${postfix} ${harnesscode})
   TARGET_LINK_LIBRARIES(harness_${postfix} YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} ${ACE_LINK_FLAGS} YARP_sig YARP_OS)
   IF (CREATE_DEVICE_LIBRARY_MODULES)
     IF (MERGE_DEVICE_LIBRARY_MODULES)
#       TARGET_LINK_LIBRARIES(harness_${postfix} yarpmod)
     ENDIF (MERGE_DEVICE_LIBRARY_MODULES)
   ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
   TARGET_LINK_LIBRARIES(harness_${postfix} YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} ${ACE_LINK_FLAGS} YARP_sig YARP_OS)
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
	SET(XDOX ${BASE_BINARY_DIR}/src/libYARP_dev/harness/${XN}.dox)
	ADD_TEST(device::${XN} ${EXECUTABLE_OUTPUT_PATH}/harness_${postfix} --file "${XINI}" --doc "${XDOX}")
    ENDFOREACH(X ${inis})
  ENDIF (CREATE_BUILTIN_DEVICE_TESTS)

ENDIF (COMPILING_ALL_YARP)

IF (NOT COMPILING_ALL_YARP)
  # make a basic test program
  SET(runner ${BASE_BINARY_DIR}${REL_DIR}generated_code/main.cpp)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/runner.cpp.in
			${runner} @ONLY  IMMEDIATE)
  ADD_EXECUTABLE(runner ${runner})
  TARGET_LINK_LIBRARIES(runner ${PROJECT_NAME} localmod)
ENDIF (NOT COMPILING_ALL_YARP)

#MESSAGE(STATUS "External modules: generated list of targets" "${YARP_EXTMOD_TARGETS}")
#MESSAGE(STATUS "External modules: generated list of libs" "${YARP_EXTMOD_LIBS}")
#MESSAGE(STATUS "YARP_dev_EXT_LIBS: " "${YARP_dev_EXT_LIBS}")
