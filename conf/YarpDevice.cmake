MACRO(YarpDevice)

SET(ADDER_CPP ${CMAKE_BINARY_DIR}/adder.cpp)
SET(ADDER_H ${CMAKE_BINARY_DIR}/adder.h)

WRITE_FILE(${ADDER_CPP} "#include \"adder.h\"")
WRITE_FILE(${ADDER_CPP} "void adder() {" APPEND)
WRITE_FILE(${ADDER_H} "")

FOREACH(dev ${ARGN})
	MESSAGE(STATUS "Dealing with device ${dev}")

	INCLUDE(${dev}/config.cmake)

	CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
	  ${CMAKE_BINARY_DIR}/add_${YARPDEV_NAME}.cpp @ONLY  IMMEDIATE)

	WRITE_FILE(${ADDER_CPP} "add_${YARPDEV_NAME}();" APPEND)
	WRITE_FILE(${ADDER_H} "extern void add_${YARPDEV_NAME}();" APPEND)

	INCLUDE_DIRECTORIES(${dev})

ENDFOREACH(dev)

WRITE_FILE(${ADDER_CPP} "}" APPEND)
WRITE_FILE(${ADDER_H} "extern void adder();" APPEND)

MESSAGE(STATUS "Generated ${ADDER_H}")
MESSAGE(STATUS "Generated ${ADDER_CPP}")

CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev.cpp.in
	${CMAKE_BINARY_DIR}/yarpdev.cpp @ONLY  IMMEDIATE)

MESSAGE(STATUS "Generated yarpdev.cpp")

ENDMACRO(YarpDevice)

