#####
# Parse a list of devices, creates library which compiles 
# all devices together. Generates code for inclusion 
# in libYARP_dev.  
# modulename: the name of the library
# path: path to the module

# April 07 -- nat

MACRO(YarpAddExtModule modulename path)

# Start a project.
PROJECT(${modulename})

## clean variables
SET(tmp_sources "")
SET(tmp_headers "")

# Check if there are any plugin device subdirectories.
# Device subdirectories contain a config.cmake file.
FILE(GLOB_RECURSE devices_list "${path}/config.cmake")
IF (devices_list)
  SET(GEN ${CMAKE_BINARY_DIR}/${modulename}/generated_code)
  IF (NOT EXISTS ${GEN})
	FILE(MAKE_DIRECTORY ${GEN})
  ENDIF (NOT EXISTS ${GEN})

  SET(devices "${devices_list}")

  # We have a cpp file and a header file that call/list the 
  # initialization methods for all devices
  SET(ADDER_CPP ${GEN}/${modulename}Adder.cpp)
  SET(ADDER_H ${GEN}/${modulename}Adder.h)
  WRITE_FILE(${ADDER_H} "")

  # Write some preamble for the cpp file and header file
  WRITE_FILE(${ADDER_CPP} "#include \"${modulename}Adder.h\"")
  WRITE_FILE(${ADDER_CPP} "void add${modulename}() {" APPEND)

  # For each device directory, create the appropriate files
  FOREACH(dev ${devices})
	# pick up the configuration of the device
	GET_FILENAME_COMPONENT(dev_path "${dev}" PATH)

	SET(SAVE_PATH ${CMAKE_MODULE_PATH})
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${dev_path})
	IF (dev_path)
	  INCLUDE(${dev_path}/config.cmake)
	ELSE (dev_path)
	  INCLUDE(${dev})
	ENDIF (dev_path)
	SET(CMAKE_MODULE_PATH ${SAVE_PATH})

	# make a flag for conditional compilation of the device
	SET(ENABLE_${YARPDEV_NAME} FALSE CACHE BOOL "Do you want to use ${YARPDEV_NAME}?")
	SET(ENABLE_YARPDEV_NAME 0)
	IF (ENABLE_${YARPDEV_NAME})
	  SET(ENABLE_YARPDEV_NAME 1)
	ENDIF (ENABLE_${YARPDEV_NAME})

	IF (ENABLE_YARPDEV_NAME)
	  # write a quick cpp file to add an appropriate factory for the device
	  CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
		${GEN}/add_${YARPDEV_NAME}.cpp @ONLY  IMMEDIATE)
	  MESSAGE(STATUS "Generated add_${YARPDEV_NAME}.cpp")

	  # aggregate this into our global list
	  WRITE_FILE(${ADDER_CPP} "add_${YARPDEV_NAME}();" APPEND)
	  WRITE_FILE(${ADDER_H} "extern void add_${YARPDEV_NAME}();" APPEND)
	  FILE(GLOB folder_dev_source ${dev_path}/*.cpp ${dev_path}/*.cc 
		${dev_path}/*.c)
	  FILE(GLOB folder_dev_header ${dev_path}/*.h)

	  SET(tmp_sources ${tmp_sources} ${GEN}/add_${YARPDEV_NAME}.cpp 
		${folder_dev_source})
	  SET(tmp_headers ${tmp_headers} ${folder_dev_header})

	  # make sure this device directory is included in our header
	  # file path
	  INCLUDE_DIRECTORIES(${dev_path})
	ELSE (ENABLE_YARPDEV_NAME)
	  MESSAGE(STATUS "Device ${YARPDEV_NAME} is not enabled")	
	ENDIF (ENABLE_YARPDEV_NAME)
  ENDFOREACH(dev)
  
  # finish up the list of devices
  WRITE_FILE(${ADDER_CPP} "}" APPEND)
  MESSAGE(STATUS "Generated ${ADDER_CPP}")
  WRITE_FILE(${ADDER_H} "extern void add${modulename}();" APPEND)
  MESSAGE(STATUS "Generated ${ADDER_H}")

  SET(tmp_sources ${tmp_sources} ${ADDER_CPP})
  SET(tmp_headers ${tmp_headers} ${ADDER_H})

  # Automatically add include directories if needed.
  FOREACH(header_file ${folder_header})
	GET_FILENAME_COMPONENT(p ${header_file} PATH)
	INCLUDE_DIRECTORIES(${p})
  ENDFOREACH(header_file ${folder_header})

  # Set up the lib
  ADD_LIBRARY(${modulename} ${tmp_sources} ${tmp_headers})
ENDIF (devices_list)
ENDMACRO(YarpAddExtModule)