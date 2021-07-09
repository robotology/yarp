# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause


add_library(YARP_sig)
add_library(YARP::YARP_sig ALIAS YARP_sig)

set(YARP_sig_HDRS yarp/sig/all.h
                  yarp/sig/api.h
                  yarp/sig/Image.h
                  yarp/sig/ImageDraw.h
                  yarp/sig/ImageFile.h
                  yarp/sig/ImageNetworkHeader.h
                  yarp/sig/ImageUtils.h
                  yarp/sig/IntrinsicParams.h
                  yarp/sig/Matrix.h
                  yarp/sig/PointCloud.h
                  yarp/sig/PointCloudBase.h
                  yarp/sig/PointCloudNetworkHeader.h
                  yarp/sig/PointCloudTypes.h
                  yarp/sig/PointCloudUtils.h
                  yarp/sig/PointCloudUtils-inl.h
                  yarp/sig/Sound.h
                  yarp/sig/SoundFile.h
                  yarp/sig/SoundFileMp3.h
                  yarp/sig/SoundFileWav.h
                  yarp/sig/Vector.h)

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  list(APPEND YARP_sig_HDRS yarp/sig/IplImage.h)
endif()

set(YARP_sig_SRCS yarp/sig/Image.cpp
                  yarp/sig/Image.copyPixels.cpp
                  yarp/sig/ImageFile.cpp
                  yarp/sig/ImageUtils.cpp
                  yarp/sig/IntrinsicParams.cpp
                  yarp/sig/Matrix.cpp
                  yarp/sig/PointCloudBase.cpp
                  yarp/sig/PointCloudUtils.cpp
                  yarp/sig/Sound.cpp
                  yarp/sig/SoundFile.cpp
                  yarp/sig/SoundFileMp3.cpp
                  yarp/sig/SoundFileWav.cpp
                  yarp/sig/Vector.cpp)

set(YARP_sig_IMPL_HDRS yarp/sig/impl/DeBayer.h
                       yarp/sig/impl/IplImage.h)

set(YARP_sig_IMPL_SRCS yarp/sig/impl/DeBayer.cpp
                       yarp/sig/impl/IplImage.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_sig_SRCS}
                   ${YARP_sig_IMPL_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_sig_HDRS}
                   ${YARP_sig_IMPL_HDRS})

target_sources(YARP_sig PRIVATE ${YARP_sig_SRCS}
                                ${YARP_sig_HDRS}
                                ${YARP_sig_IMPL_SRCS}
                                ${YARP_sig_IMPL_HDRS})

target_include_directories(YARP_sig PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_sig PUBLIC cxx_std_14)

target_link_libraries(YARP_sig PUBLIC YARP::YARP_conf
                                      YARP::YARP_os)
list(APPEND YARP_sig_PUBLIC_DEPS YARP_conf
                                 YARP_os)

# YARP_sig library uses headers from YARP_os impl, and therefore ACE headers
# are also required.
if(YARP_HAS_ACE)
  target_compile_definitions(YARP_sig PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_sig PRIVATE ACE::ACE_INLINE)
  list(APPEND YARP_sig_PRIVATE_DEPS ACE)
endif()

if(YARP_HAS_JPEG)
  target_include_directories(YARP_sig SYSTEM PRIVATE ${JPEG_INCLUDE_DIR})
  target_compile_definitions(YARP_sig PRIVATE YARP_HAS_JPEG_C=1)
  target_link_libraries(YARP_sig PRIVATE ${JPEG_LIBRARY})
  list(APPEND YARP_sig_PRIVATE_DEPS JPEG)
endif()

if(YARP_HAS_ZLIB)
  target_include_directories(YARP_sig SYSTEM PRIVATE ${ZLIB_INCLUDE_DIR})
  target_compile_definitions(YARP_sig PRIVATE YARP_HAS_ZLIB)
  target_link_libraries(YARP_sig PRIVATE ${ZLIB_LIBRARY})
  list(APPEND YARP_sig_PRIVATE_DEPS ZLIB)
endif()

if(YARP_HAS_PNG)
  target_include_directories(YARP_sig SYSTEM PRIVATE ${PNG_INCLUDE_DIR})
  target_compile_definitions(YARP_sig PRIVATE YARP_HAS_PNG)
  target_link_libraries(YARP_sig PRIVATE ${PNG_LIBRARY})
  list(APPEND YARP_sig_PRIVATE_DEPS PNG)
endif()

if (YARP_HAS_FFMPEG)
  target_include_directories(YARP_sig SYSTEM PRIVATE ${FFMPEG_avcodec_INCLUDE_DIRS}
                                                     ${FFMPEG_avutil_INCLUDE_DIRS})
  target_compile_definitions(YARP_sig PRIVATE YARP_HAS_FFMPEG)
  target_link_libraries(YARP_sig PRIVATE ${FFMPEG_avcodec_LIBRARIES}
                                         ${FFMPEG_avutil_LIBRARIES})
  # list(APPEND YARP_sig_PRIVATE_DEPS FFMPEG) # Not using targets
endif()

set_property(TARGET YARP_sig PROPERTY PUBLIC_HEADER ${YARP_sig_HDRS})
set_property(TARGET YARP_sig PROPERTY PRIVATE_HEADER ${YARP_sig_IMPL_HDRS})
set_property(TARGET YARP_sig PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_sig PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_sig PROPERTY FOLDER "Libraries")


install(TARGETS YARP_sig
        EXPORT YARP_sig
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_sig
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_sig
          NAMELINK_COMPONENT YARP_sig-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_sig-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig"
          COMPONENT YARP_sig-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/sig/impl"
          COMPONENT YARP_sig-priv-dev)


set(YARP_sig_PUBLIC_DEPS ${YARP_sig_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_sig_PRIVATE_DEPS ${YARP_sig_PRIVATE_DEPS} PARENT_SCOPE)
