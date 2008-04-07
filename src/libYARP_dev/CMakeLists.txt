PROJECT(libYARP_dev)
SET(PROJECT_NAME YARP_dev)

## COMMON INCLUDE FILES
INCLUDE_DIRECTORIES(include ${CMAKE_SOURCE_DIR}/src/libYARP_dev/include 
  ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${CMAKE_SOURCE_DIR}/src/libYARP_sig/include ${ACE_INCLUDE_DIR})

SET(populator ${CMAKE_BINARY_DIR}/src/libYARP_dev/src_generated/PopulateDrivers.cpp)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp.in
  ${populator} @ONLY  IMMEDIATE)

SET(NEED_LIBS YARP_OS YARP_sig)
SET(NEED_SRCS ${populator})
YarpLibrary(YARP_dev dev)

# Add in hardware specific tests, if requested
IF (CREATE_BUILTIN_DEVICE_TESTS)
  # add in our tests
  FILE(GLOB inis harness/*.ini)
  FOREACH(X ${inis})
    GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
    GET_FILENAME_COMPONENT(XINI ${X} ABSOLUTE)
    SET(DOC_DIR "${CMAKE_BINARY_DIR}/src/libYARP_dev/harness")
    IF (NOT EXISTS ${DOC_DIR})
  	FILE(MAKE_DIRECTORY ${DOC_DIR})
    ENDIF (NOT EXISTS ${DOC_DIR})
    SET(XDOX ${DOC_DIR}/${XN}.dox)
    ADD_TEST(device::${XN} ${EXECUTABLE_OUTPUT_PATH}/harness_dev --file "${XINI}" --doc "${XDOX}")
  ENDFOREACH(X ${inis})
ENDIF (CREATE_BUILTIN_DEVICE_TESTS)

IF(MSVC)
    SET_TARGET_PROPERTIES(YARP_dev PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)
