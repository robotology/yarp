# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if (YARP_COMPILE_libYARP_math)

  project(YARP_math)

  set(YARP_math_HDRS include/yarp/math/api.h
                     include/yarp/math/Math.h
                     include/yarp/math/NormRand.h
                     include/yarp/math/Rand.h
                     include/yarp/math/RandnScalar.h
                     include/yarp/math/RandnVector.h
                     include/yarp/math/RandScalar.h
                     include/yarp/math/RandVector.h
                     include/yarp/math/SVD.h
                     include/yarp/math/Quaternion.h
                     include/yarp/math/Vec2D.h
                     include/yarp/math/FrameTransform.h)

  set(YARP_math_IMPL_HDRS)

  set(YARP_math_SRCS src/math.cpp
                     src/NormRand.cpp
                     src/Rand.cpp
                     src/RandnVector.cpp
                     src/RandVector.cpp
                     src/SVD.cpp
                     src/Quaternion.cpp
                     src/Vec2D.cpp
                     src/FrameTransform.cpp
                     src/RandScalar.cpp
                     src/RandnScalar.cpp)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${YARP_math_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${YARP_math_HDRS}
                     ${YARP_math_IMPL_HDRS})

  add_library(YARP_math ${YARP_math_SRCS}
                        ${YARP_math_HDRS}
                        ${YARP_math_IMPL_HDRS})
  add_library(YARP::YARP_math ALIAS YARP_math)

  target_include_directories(YARP_math PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

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

  # import math symbols from standard cmath
  target_compile_definitions(YARP_math PRIVATE _USE_MATH_DEFINES)

  target_compile_features(YARP_math PUBLIC cxx_override)

  set_property(TARGET YARP_math PROPERTY PUBLIC_HEADER ${YARP_math_HDRS})
  set_property(TARGET YARP_math PROPERTY PRIVATE_HEADER ${YARP_math_IMPL_HDRS})
  set_property(TARGET YARP_math PROPERTY VERSION ${YARP_VERSION_SHORT})
  set_property(TARGET YARP_math PROPERTY SOVERSION ${YARP_SOVERSION})

  install(TARGETS YARP_math
          EXPORT YARP_math
          COMPONENT YARP_math
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math"
          PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math/impl")

  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_math
                                   DEPENDENCIES ${YARP_math_PUBLIC_DEPS}
                                   PRIVATE_DEPENDENCIES ${YARP_math_PRIVATE_DEPS})

  set_property(TARGET YARP_math PROPERTY FOLDER "Libraries")
endif()
