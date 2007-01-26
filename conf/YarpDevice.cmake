MACRO(YarpDevice)

# We have a cpp file and a header file that call/list the 
# initialization methods for all devices
SET(ADDER_CPP ${CMAKE_BINARY_DIR}/adder.cpp)
SET(ADDER_H ${CMAKE_BINARY_DIR}/adder.h)

# Write some preamble for the cpp file and header file
WRITE_FILE(${ADDER_CPP} "#include \"adder.h\"")
WRITE_FILE(${ADDER_CPP} "void adder() {" APPEND)
WRITE_FILE(${ADDER_H} "")

# For each device directory, create the appropriate files
FOREACH(dev ${ARGN})
	MESSAGE(STATUS "Dealing with device ${dev}")

	# pick up the configuration of the device
	GET_FILENAME_COMPONENT(dev_path "${dev}" PATH)
	INCLUDE(${dev_path}/config.cmake)

	# make a flag for conditional compilation of the device
	SET(ENABLE_${YARPDEV_NAME} TRUE CACHE BOOL "Do you want to use ${YARPDEV_NAME}?")
	SET(ENABLE_YARPDEV_NAME 0)
	IF (ENABLE_${YARPDEV_NAME})
	  SET(ENABLE_YARPDEV_NAME 1)
	ENDIF (ENABLE_${YARPDEV_NAME})

	# write a quick cpp file to add an appropriate factory for the device
	CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
	  ${CMAKE_BINARY_DIR}/add_${YARPDEV_NAME}.cpp @ONLY  IMMEDIATE)
	MESSAGE(STATUS "Generated add_${YARPDEV_NAME}.cpp")

	# aggregate this into our global list
	WRITE_FILE(${ADDER_CPP} "add_${YARPDEV_NAME}();" APPEND)
	WRITE_FILE(${ADDER_H} "extern void add_${YARPDEV_NAME}();" APPEND)

	# make sure this device directory is included in our header
	# file path
	INCLUDE_DIRECTORIES(${dev_path})
	
ENDFOREACH(dev)

# finish up the list of devices
WRITE_FILE(${ADDER_CPP} "}" APPEND)
MESSAGE(STATUS "Generated ${ADDER_CPP}")
WRITE_FILE(${ADDER_H} "extern void adder();" APPEND)
MESSAGE(STATUS "Generated ${ADDER_H}")

# write a standard wrapper program for creating the devices
CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev.cpp.in
	${CMAKE_BINARY_DIR}/yarpdev.cpp @ONLY  IMMEDIATE)
MESSAGE(STATUS "Generated yarpdev.cpp")

# we are done!

ENDMACRO(YarpDevice)

