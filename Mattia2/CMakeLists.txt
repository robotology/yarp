PROJECT(pumakinlib)


INCLUDE_DIRECTORIES(${YARP_DIR}/src/libYARP_math/include ${YARP_DIR}/src/libYARP_sig/include/ ${YARP_DIR}/src/libYARP_OS/include ${YARP_DIR}/src/libYARP_dev/include ${YARP_DIR}/src/libYARP_dev/src/mei/common )



SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
ADD_LIBRARY(pumakinlib PumaKinematics.h PumaKinematics.cpp)




