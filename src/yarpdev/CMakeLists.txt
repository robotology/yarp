
INCLUDE_DIRECTORIES(${YARP_OS_INC} ${YARP_sig_INC} ${YARP_dev_INC})
LINK_LIBRARIES(YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)

# prepare for devices that are linked externally to libYARP_dev
INCLUDE(YarpDevice)
IF (CREATE_DEVICE_LIBRARY_MODULES)
    # add in device library
    SET(yarpmod_DIR ${CMAKE_BINARY_DIR})
    IMPORT_DEVICES(${CMAKE_BINARY_DIR}/drivers.h yarpmod)
ELSE (CREATE_DEVICE_LIBRARY_MODULES)
    # no device library to import, create blank stup
    IMPORT_DEVICES(${CMAKE_BINARY_DIR}/drivers.h)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})


########################################################################
##
## yarpdev -- command line control of devices
##

ADD_EXECUTABLE(yarpdev yarpdev.cpp)
TARGET_LINK_LIBRARIES(yarpdev YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)


########################################################################
##
## yarphear -- command line access to audio
##

ADD_EXECUTABLE(yarphear yarphear.cpp)
TARGET_LINK_LIBRARIES(yarphear YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)


########################################################################
##
## yarpview -- visualize image streams
##

SET(YARP_ADMIN "$ENV{YARP_ADMIN}")
IF (YARP_ADMIN)
    ADD_EXECUTABLE(yarpview2 yarpview.cpp)
    TARGET_LINK_LIBRARIES(yarpview2 YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
ENDIF (YARP_ADMIN)

INSTALL_TARGETS(/bin yarpdev yarphear)
