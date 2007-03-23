PROJECT (Examples)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
FIND_PACKAGE(GSL)
IF(GSL_FOUND)
  INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
  
  ADD_EXECUTABLE(basic_ops basic_ops.cpp)
  ADD_EXECUTABLE(gsl_example gsl_example.cpp)

  TARGET_LINK_LIBRARIES(gsl_example ${GSL_LIBRARIES})
  TARGET_LINK_LIBRARIES(basic_ops ${GSL_LIBRARIES})
ENDIF(GSL_FOUND)

