# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET(NEED_LIBS "")

OPTION(CREATE_MPI_CARRIER "Support communication via MPI" OFF)

IF (CREATE_MPI_CARRIER)
  # Search for mpi library to support MpiStream
  FIND_PACKAGE(MPI REQUIRED)
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
  ADD_DEFINITIONS(-DCREATE_MPI_CARRIER)

  # include print statements for debugging purpose
  OPTION(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF)
  IF (MPI_DEBUG_MSG)
    ADD_DEFINITIONS(-DMPI_DEBUG)
  ENDIF (MPI_DEBUG_MSG)

ENDIF (CREATE_MPI_CARRIER)


YarpLibrary(YARP_OS os)

# link yarp_os lib against mpi
TARGET_LINK_LIBRARIES(YARP_OS ${MPI_LIBRARIES})

# add YARP Network companion executable
ADD_EXECUTABLE(yarp yarp/yarp.cpp)
ADD_EXECUTABLE(yarpserver yarp/yarpserver.cpp)
TARGET_LINK_LIBRARIES(yarp YARP_OS ${ACE_LINK_FLAGS})
TARGET_LINK_LIBRARIES(yarpserver YARP_OS ${ACE_LINK_FLAGS})
INSTALL_TARGETS(/bin yarp yarpserver)

IF(MSVC)
    SET_TARGET_PROPERTIES(yarp PROPERTIES COMPILE_FLAGS /wd4996)
    SET_TARGET_PROPERTIES(yarpserver PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)
 
