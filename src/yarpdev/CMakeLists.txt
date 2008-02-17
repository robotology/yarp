INCLUDE_DIRECTORIES(${YARP_OS_INC} ${YARP_sig_INC} ${YARP_dev_INC})
LINK_LIBRARIES(YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)

# prepare for devices that are linked externally to libYARP_dev
INCLUDE(YarpDevice)
IF (CREATE_DEVICE_LIBRARY_MODULES)
    # add in device library
    SET(yarpmod_DIR ${CMAKE_BINARY_DIR})
    #IMPORT_DEVICES(${CMAKE_BINARY_DIR}/drivers.h yarpmod)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

########################################################################
##
## yarpdev -- command line control of devices
##

ADD_EXECUTABLE(yarpdev yarpdev.cpp)
IF (CREATE_DEVICE_LIBRARY_MODULES)
  TARGET_IMPORT_DEVICES(yarpdev
  		      ${CMAKE_BINARY_DIR}/yarpdev_drivers.h yarpmod)
ELSE (CREATE_DEVICE_LIBRARY_MODULES)
  TARGET_IMPORT_DEVICES(yarpdev
  		      ${CMAKE_BINARY_DIR}/yarpdev_drivers.h)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)

TARGET_LINK_LIBRARIES(yarpdev YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
IF(WIN32 AND NOT CYGWIN)
    SET_TARGET_PROPERTIES(yarpdev PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(WIN32 AND NOT CYGWIN)

########################################################################
##
## yarphear -- command line access to audio
##

ADD_EXECUTABLE(yarphear yarphear.cpp)
IF (CREATE_DEVICE_LIBRARY_MODULES)
  #BEGIN_DEVICE_LIBRARY(yarphearmod) 
  #SET(COMPILE_BY_DEFAULT TRUE)
  #ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/portaudio
  #                 ${CMAKE_CURRENT_BINARY_DIR})
  #END_DEVICE_LIBRARY(yarphearmod)
  TARGET_IMPORT_DEVICES(yarphear
		      ${CMAKE_BINARY_DIR}/yarphear_drivers.h yarpmod)
ELSE (CREATE_DEVICE_LIBRARY_MODULES)
  TARGET_IMPORT_DEVICES(yarphear
		      ${CMAKE_BINARY_DIR}/yarphear_drivers.h)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
#TARGET_LINK_LIBRARIES(yarphear YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
IF(WIN32 AND NOT CYGWIN)
    SET_TARGET_PROPERTIES(yarphear PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(WIN32 AND NOT CYGWIN)

########################################################################
##
## yarpview -- visualize image streams
##

SET(YARP_ADMIN "$ENV{YARP_ADMIN}")
#SET(YARP_ADMIN FALSE)
IF (YARP_ADMIN)
    #BEGIN_DEVICE_LIBRARY(viewmod) 
    #SET(COMPILE_BY_DEFAULT TRUE)
    #ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/modules/wxsdl
    #                 ${CMAKE_CURRENT_BINARY_DIR})
    #END_DEVICE_LIBRARY(viewmod)
    ADD_EXECUTABLE(yarpview2 yarpview.cpp)
    IF (CREATE_DEVICE_LIBRARY_MODULES)
      TARGET_IMPORT_DEVICES(yarpview2 
		          ${CMAKE_BINARY_DIR}/yarpview_drivers.h yarpmod)
    ELSE (CREATE_DEVICE_LIBRARY_MODULES)
      TARGET_IMPORT_DEVICES(yarpview2 
		          ${CMAKE_BINARY_DIR}/yarpview_drivers.h)
    ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
    #TARGET_LINK_LIBRARIES(yarpview2 yarpmod)
    #TARGET_LINK_LIBRARIES(yarpview2 YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
    IF(WIN32 AND NOT CYGWIN)
        SET_TARGET_PROPERTIES(yarpview2 PROPERTIES COMPILE_FLAGS /wd4996)
    ENDIF(WIN32 AND NOT CYGWIN)
    
ENDIF (YARP_ADMIN)

INSTALL_TARGETS(/bin yarpdev yarphear)
