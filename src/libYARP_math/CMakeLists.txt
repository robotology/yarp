# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set(YARP_HAS_MATH_LIB FALSE)
if (CREATE_LIB_MATH)
  set(YARP_HAS_MATH_LIB TRUE)
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
                     include/yarp/math/SVD.h
                     include/yarp/math/Quaternion.h
                     include/yarp/math/Vec2D.h
                     include/yarp/math/FrameTransform.h)

  set(YARP_math_IMPL_HDRS include/yarp/math/impl/mt.h)

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
                     src/RandnScalar.cpp
                     src/mt.cc)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${YARP_math_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${YARP_math_HDRS}
                     ${YARP_math_IMPL_HDRS})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

  # import math symbols from standard cmath
  add_definitions(-D_USE_MATH_DEFINES)

  add_library(YARP_math ${YARP_math_SRCS}
                        ${YARP_math_HDRS}
                        ${YARP_math_IMPL_HDRS})
  add_library(YARP::YARP_math ALIAS YARP_math)

  target_link_libraries(YARP_math PRIVATE YARP::YARP_OS)
  target_link_libraries(YARP_math PRIVATE YARP::YARP_sig)

  set_property(TARGET YARP_math PROPERTY PUBLIC_HEADER ${YARP_math_HDRS})
  set_property(TARGET YARP_math PROPERTY PRIVATE_HEADER ${YARP_math_IMPL_HDRS})

  install(TARGETS YARP_math
          EXPORT YARP
          COMPONENT runtime
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math"
          PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/math/impl")

  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_math)
  set_property(TARGET YARP_math PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
  set_property(TARGET YARP_math PROPERTY FOLDER "Libraries")
endif()

set_property(GLOBAL PROPERTY YARP_HAS_MATH_LIB ${YARP_HAS_MATH_LIB})
