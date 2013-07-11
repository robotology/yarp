# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_sig)

# TODO  src/libYARP_sig/include/yarp/gsl_compatibility.h

set(YARP_sig_HDRS include/yarp/sig/all.h
                  include/yarp/sig/api.h
                  include/yarp/sig/ImageDraw.h
                  include/yarp/sig/ImageFile.h
                  include/yarp/sig/Image.h
                  include/yarp/sig/ImageNetworkHeader.h
                  include/yarp/sig/IplImage.h
                  include/yarp/sig/Matrix.h
                  include/yarp/sig/SoundFile.h
                  include/yarp/sig/Sound.h
                  include/yarp/sig/Vector.h)

set(YARP_sig_SRCS src/ImageCopy.cpp
                  src/Image.cpp
                  src/ImageFile.cpp
                  src/IplImage.cpp
                  src/Matrix.cpp
                  src/Sound.cpp
                  src/SoundFile.cpp
                  src/Vector.cpp)

source_group("Source Files" FILES ${YARP_sig_SRCS})
source_group("Header Files" FILES ${YARP_sig_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS
                ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})

add_library(YARP_sig ${YARP_sig_SRCS} ${YARP_sig_HDRS})
target_link_libraries(YARP_sig LINK_PRIVATE YARP_OS)
install(TARGETS YARP_sig EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_sig)

set_property(TARGET YARP_sig PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(FILES ${YARP_sig_HDRS}
        COMPONENT development
        DESTINATION include/yarp/sig)
