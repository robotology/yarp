PROJECT(configuration)

#GET_TARGET_PROPERTY(YARP_dev_LIB YARP_dev LOCATION)
#GET_TARGET_PROPERTY(YARP_dev_INC YARP_dev header_path)
#GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS YARP_dev ext_libs)
# actually device stuff isn't set up yet

## Remember: on Linux order does matter!
INCLUDE(YarpLibraryNames)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)


# There are several macros we would like to have available
# for user

FOREACH(conf YarpDevice.cmake YarpLibraryNames.cmake YarpModuleHooks.cmake FindOpenCV.cmake FindGtkPlus.cmake FindGSL.cmake FindPortAudio.cmake FindYarp1.cmake FindGtkWin32.cmake FindFFMPEG.cmake FindBoost.cmake FindAce.cmake)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/${conf}
                 ${CMAKE_BINARY_DIR}/conf/${conf} COPYONLY IMMEDIATE)
ENDFOREACH(conf)

