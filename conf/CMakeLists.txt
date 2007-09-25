PROJECT(configuration)

#GET_TARGET_PROPERTY(YARP_dev_LIB YARP_dev LOCATION)
#GET_TARGET_PROPERTY(YARP_dev_INC YARP_dev header_path)
#GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS YARP_dev ext_libs)
# actually device stuff isn't set up yet

## Remember: on Linux order does matter!
IF(YARP_HAS_MATH_LIB)
SET(YARP_LIBRARIES "${YARP_dev_LIB};${YARP_EXTMOD_LIBS};${YARP_dev_EXT_LIBS};${YARP_math_LIB};${YARP_sig_compat_LIB};${YARP_OS_compat_LIB};${YARP_sig_LIB};${YARP_OS_LIB};${ACE_LIB}")
SET(YARP_INCLUDES "${ACE_INCLUDE_DIR};${YARP_OS_INC};${YARP_sig_INC};${YARP_math_INC};${YARP_dev_INC};${YARP_OS_compat_INC}")
ELSE(YARP_HAS_MATH_LIB)
SET(YARP_LIBRARIES "${YARP_dev_LIB};${YARP_EXTMOD_LIBS};${YARP_dev_EXT_LIBS};${YARP_sig_compat_LIB};${YARP_OS_compat_LIB};${YARP_sig_LIB};${YARP_OS_LIB};${ACE_LIB}")
SET(YARP_INCLUDES "${ACE_INCLUDE_DIR};${YARP_OS_INC};${YARP_sig_INC};${YARP_dev_INC};${YARP_OS_compat_INC}")
ENDIF(YARP_HAS_MATH_LIB)


CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)


# There are several macros we would like to have available
# for user

FOREACH(conf YarpCompileModules.cmake FindOpenCV.cmake FindGtkPlus.cmake FindGSL.cmake FindPortAudio.cmake FindYarp1.cmake FindGtkWin32.cmake FindFFMPEG.cmake FindBoost.cmake FindAce.cmake)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/${conf}
                 ${CMAKE_BINARY_DIR}/conf/${conf} COPYONLY IMMEDIATE)
ENDFOREACH(conf)

