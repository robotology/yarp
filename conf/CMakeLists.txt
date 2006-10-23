PROJECT(configuration)

#GET_TARGET_PROPERTY(YARP_dev_LIB YARP_dev LOCATION)
#GET_TARGET_PROPERTY(YARP_dev_INC YARP_dev header_path)
#GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS YARP_dev ext_libs)
# actually device stuff isn't set up yet

SET(YARP_LIBRARIES "${YARP_dev_LIB};${YARP_sig_compat_LIB};${YARP_OS_compat_LIB};${YARP_sig_LIB};${YARP_OS_LIB};${ACE_LIB}")
SET(YARP_INCLUDES "${ACE_INCLUDE_DIR};${YARP_OS_INC};${YARP_sig_INC};${YARP_dev_INC};${YARP_OS_compat_INC}")

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)

#MESSAGE(STATUS "YARPConfig.make records libs as: " ${YARP_LIBRARIES})
#MESSAGE(STATUS "YARPConfig.make records includes as: " ${YARP_INCLUDES})


