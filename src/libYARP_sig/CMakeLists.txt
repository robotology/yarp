# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_sig)

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

set(YARP_sig_IMPL_HDRS include/yarp/sig/impl/DeBayer.h)

set(YARP_sig_SRCS src/ImageCopy.cpp
                  src/Image.cpp
                  src/ImageFile.cpp
                  src/IplImage.cpp
                  src/Matrix.cpp
                  src/Sound.cpp
                  src/SoundFile.cpp
                  src/Vector.cpp
                  src/DeBayer.cpp)

source_group("Source Files" FILES ${YARP_sig_SRCS})
source_group("Header Files" FILES ${YARP_sig_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
# YARP_sig library uses headers from YARP_OS impl, and therefore ACE headers
# are also required.
include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_library(YARP_sig ${YARP_sig_SRCS} ${YARP_sig_HDRS})
target_link_libraries(YARP_sig LINK_PRIVATE YARP_OS ${ACE_LIBRARIES})

set_property(TARGET YARP_sig PROPERTY PUBLIC_HEADER ${YARP_sig_HDRS})
set_property(TARGET YARP_sig PROPERTY PRIVATE_HEADER ${YARP_sig_IMPL_HDRS})

install(TARGETS YARP_sig
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig/impl)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_sig)
set_property(TARGET YARP_sig PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
