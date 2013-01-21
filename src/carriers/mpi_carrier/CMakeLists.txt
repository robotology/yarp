# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Daniel Krieg
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_PLUGIN_LIBRARY)
  PREPARE_CARRIER(mpi_carrier TYPE MpiP2PCarrier INCLUDE include/yarp/os/impl/MpiP2PCarrier.h)
  PREPARE_CARRIER(mpibcast_carrier TYPE MpiBcastCarrier INCLUDE include/yarp/os/impl/MpiBcastCarrier.h)
  YARP_ADD_CARRIER_FINGERPRINT(mpi.ini mpi_carrier mpibcast_carrier)
ENDIF (COMPILE_PLUGIN_LIBRARY)

IF (NOT SKIP_mpi_carrier)

  FIND_PACKAGE(MPI REQUIRED)
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
  ADD_DEFINITIONS(-DCREATE_MPI_CARRIER)

  # include print statements for debugging purpose
  OPTION(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF)
  IF (MPI_DEBUG_MSG)
    ADD_DEFINITIONS(-DMPI_DEBUG)
  ENDIF (MPI_DEBUG_MSG)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
  ADD_LIBRARY(yarp_mpi
    src/MpiCarrier.cpp
    src/MpiComm.cpp
    src/MpiStream.cpp src/MpiP2PStream.cpp
    include/yarp/os/impl/MpiCarrier.h include/yarp/os/impl/MpiP2PCarrier.h
    include/yarp/os/impl/MpiStream.h include/yarp/os/impl/MpiP2PStream.h
    include/yarp/os/impl/MpiComm.h
  )
  TARGET_LINK_LIBRARIES(yarp_mpi ${MPI_LIBRARIES} ${YARP_LIBRARIES})

ENDIF (NOT SKIP_mpi_carrier)

IF (NOT SKIP_mpibcast_carrier)

  FIND_PACKAGE(MPI REQUIRED)
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
  ADD_DEFINITIONS(-DCREATE_MPI_CARRIER)

  # include print statements for debugging purpose
  OPTION(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF)
  IF (MPI_DEBUG_MSG)
    ADD_DEFINITIONS(-DMPI_DEBUG)
  ENDIF (MPI_DEBUG_MSG)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
  ADD_LIBRARY(yarp_mpibcast
    src/MpiCarrier.cpp src/MpiBcastCarrier.cpp
    src/MpiComm.cpp
    src/MpiStream.cpp src/MpiBcastStream.cpp
    include/yarp/os/impl/MpiCarrier.h include/yarp/os/impl/MpiBcastCarrier.h
    include/yarp/os/impl/MpiStream.h include/yarp/os/impl/MpiBcastStream.h
    include/yarp/os/impl/MpiComm.h
  )
  TARGET_LINK_LIBRARIES(yarp_mpibcast ${MPI_LIBRARIES} ${YARP_LIBRARIES})

ENDIF (NOT SKIP_mpibcast_carrier)
