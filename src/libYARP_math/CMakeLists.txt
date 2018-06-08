# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(YARP_HAS_MATH_LIB FALSE PARENT_SCOPE)
if (CREATE_LIB_MATH)
  set(YARP_HAS_MATH_LIB TRUE PARENT_SCOPE)
endif()

if (YARP_HAS_MATH_LIB)

  project(YARP_math)

  set(YARP_math_HDRS include/yarp/math/api.h
                     include/yarp/math/Math.h
                     include/yarp/math/NormRand.h
                     include/yarp/math/Rand.h
                     include/yarp/math/RandnScalar.h
                     include/yarp/math/RandnVector.h
                     include/yarp/math/RandScalar.h
                     include/yarp/math/RandVector.h
                     include/yarp/math/SVD.h)

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
                                         YARP::YARP_OS
                                         YARP::YARP_sig
                                  PRIVATE YARP::YARP_eigen)
  list(APPEND YARP_math_PUBLIC_DEPS YARP_conf
                                    YARP_OS
                                    YARP_sig)
  list(APPEND YARP_math_PRIVATE_DEPS YARP_eigen)

  # NOTE: Fix this when Eigen3 3.3.1 is available on all supported distributions
  if(TARGET Eigen3::Eigen)
    target_link_libraries(YARP_math PRIVATE Eigen3::Eigen)
    list(APPEND YARP_math_PRIVATE_DEPS "Eigen3 3.3.1")
  else()
    target_include_directories(YARP_math SYSTEM PRIVATE ${EIGEN3_INCLUDE_DIR})
  endif()

  # import math symbols from standard cmath
  target_compile_definitions(YARP_math PRIVATE _USE_MATH_DEFINES)

  if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(CMAKE_VERSION VERSION_LESS 3.1)
    if(DEFINED CXX11_FLAGS)
      target_compile_options(YARP_math PUBLIC ${CXX11_FLAGS})
    endif()
  else()
    target_compile_features(YARP_math PUBLIC cxx_override)
  endif()

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
