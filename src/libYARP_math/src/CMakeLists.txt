# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_math)
add_library(YARP::YARP_math ALIAS YARP_math)

set(YARP_math_HDRS yarp/math/api.h
                   yarp/math/Math.h
                   yarp/math/NormRand.h
                   yarp/math/Rand.h
                   yarp/math/RandnScalar.h
                   yarp/math/RandnVector.h
                   yarp/math/RandScalar.h
                   yarp/math/RandVector.h
                   yarp/math/SVD.h
                   yarp/math/Quaternion.h
                   yarp/math/Vec2D.h
                   yarp/math/FrameTransform.h)

set(YARP_math_IMPL_HDRS)

set(YARP_math_SRCS yarp/math/math.cpp
                   yarp/math/NormRand.cpp
                   yarp/math/Rand.cpp
                   yarp/math/RandnVector.cpp
                   yarp/math/RandVector.cpp
                   yarp/math/SVD.cpp
                   yarp/math/Quaternion.cpp
                   yarp/math/Vec2D.cpp
                   yarp/math/FrameTransform.cpp
                   yarp/math/RandScalar.cpp
                   yarp/math/RandnScalar.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_math_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_math_HDRS}
                   ${YARP_math_IMPL_HDRS})

target_sources(YARP_math PRIVATE ${YARP_math_SRCS}
                                 ${YARP_math_HDRS}
                                 ${YARP_math_IMPL_HDRS})

target_include_directories(YARP_math PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_math PUBLIC cxx_std_14)

# import math symbols from standard cmath
target_compile_definitions(YARP_math PRIVATE _USE_MATH_DEFINES)

target_link_libraries(YARP_math PUBLIC YARP::YARP_conf
                                       YARP::YARP_os
                                       YARP::YARP_sig
                                PRIVATE YARP::YARP_eigen)
list(APPEND YARP_math_PUBLIC_DEPS YARP_conf
                                  YARP_os
                                  YARP_sig)
list(APPEND YARP_math_PRIVATE_DEPS YARP_eigen)

target_link_libraries(YARP_math PRIVATE Eigen3::Eigen)
list(APPEND YARP_math_PRIVATE_DEPS Eigen3)

set_property(TARGET YARP_math PROPERTY PUBLIC_HEADER ${YARP_math_HDRS})
set_property(TARGET YARP_math PROPERTY PRIVATE_HEADER ${YARP_math_IMPL_HDRS})
set_property(TARGET YARP_math PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_math PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_math PROPERTY FOLDER "Libraries")


install(TARGETS YARP_math
        EXPORT YARP_math
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_math
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_math
          NAMELINK_COMPONENT YARP_math-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_math-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math"
          COMPONENT YARP_math-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math/impl"
          COMPONENT YARP_math-priv-dev)


set(YARP_math_PUBLIC_DEPS ${YARP_math_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_math_PRIVATE_DEPS ${YARP_math_PRIVATE_DEPS} PARENT_SCOPE)
