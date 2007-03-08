PROJECT (Examples)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
FIND_PACKAGE(GSL)

ADD_EXECUTABLE(basic_ops basic_ops.cpp)

IF(GSL_FOUND)
  MESSAGE(STATUS "Found gsl, adding GSL related examples")

  INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

  ADD_EXECUTABLE(gsl_example gsl_example.cpp)

  TARGET_LINK_LIBRARIES(gsl_example ${GSL_LIBRARIES})
ENDIF(GSL_FOUND)

