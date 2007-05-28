MACRO(YarpDevice)

INCLUDE(UsePkgConfig)

SET(GEN ${CMAKE_BINARY_DIR}/generated_code)
IF (NOT EXISTS ${GEN})
	FILE(MAKE_DIRECTORY ${GEN})
ENDIF (NOT EXISTS ${GEN})

# We have a cpp file and a header file that call/list the 
# initialization methods for all devices
SET(ADDER_CPP ${GEN}/adder.cpp)
SET(ADDER_H ${GEN}/adder.h)

# Write some preamble for the cpp file and header file
WRITE_FILE(${ADDER_CPP} "#include \"adder.h\"")
WRITE_FILE(${ADDER_CPP} "void adder() {" APPEND)
WRITE_FILE(${ADDER_H} "")

# For each device directory, create the appropriate files
FOREACH(dev ${ARGN})
	MESSAGE(STATUS "Dealing with device ${dev}")

	# pick up the configuration of the device
	GET_FILENAME_COMPONENT(dev_path "${dev}" PATH)
	SET(SAVE_PATH ${CMAKE_MODULE_PATH})
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${dev_path})
	IF (dev_path)
		INCLUDE(${dev_path}/yarpdevice.cmake)
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
		FILE(GLOB_RECURSE folder_dev_source ${dev_path}/*.cpp ${dev_path}/*.cc 
			${dev_path}/*.c)
	    	FILE(GLOB_RECURSE folder_dev_header ${dev_path}/*.h)
	
		SET(folder_source ${folder_source} ${folder_dev_source}
			${GEN}/add_${YARPDEV_NAME}.cpp)
		SET(folder_header ${folder_header} ${folder_dev_header})
	
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
WRITE_FILE(${ADDER_H} "extern void adder();" APPEND)
MESSAGE(STATUS "Generated ${ADDER_H}")
SET(folder_source ${folder_source} ${ADDER_CPP})
SET(folder_header ${folder_header} ${ADDER_H})

# write a standard wrapper program for creating the devices
CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev.cpp.in
	${GEN}/yarpdev.cpp @ONLY  IMMEDIATE)
MESSAGE(STATUS "Generated yarpdev.cpp")
SET(folder_source ${folder_source} ${GEN}/yarpdev.cpp)

# we are done!

ENDMACRO(YarpDevice)

