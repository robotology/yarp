PROJECT(configuration)

#GET_TARGET_PROPERTY(YARP_dev_LIB YARP_dev LOCATION)
#GET_TARGET_PROPERTY(YARP_dev_INC YARP_dev header_path)
#GET_TARGET_PROPERTY(YARP_dev_EXT_LIBS YARP_dev ext_libs)
# actually device stuff isn't set up yet

## Remember: on Linux order does matter!
INCLUDE(YarpLibraryNames)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)
FOREACH(conf version system options)
  SET(outconf ${CMAKE_BINARY_DIR}/include/yarp/conf/${conf}.h)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/yarp_config_${conf}.h.in
                 ${outconf} IMMEDIATE)
  MESSAGE(STATUS "generated ${outconf}")
  INSTALL_FILES(/include/yarp/conf FILES ${outconf})
ENDFOREACH(conf)


CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)

# There are several macros we would like to have available
# for user

FOREACH(conf YarpDevice.cmake YarpLibraryNames.cmake YarpModuleHooks.cmake FindOpenCV.cmake FindGtkPlus.cmake FindGSL.cmake FindPortAudio.cmake FindGtkWin32.cmake FindFFMPEG.cmake FindBoost.cmake FindAce.cmake)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/${conf}
                 ${CMAKE_BINARY_DIR}/conf/${conf} COPYONLY IMMEDIATE)
ENDFOREACH(conf)

