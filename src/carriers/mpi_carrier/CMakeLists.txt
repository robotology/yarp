# Copyright (C) 2010 Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_PLUGIN_LIBRARY)
  PREPARE_CARRIER(mpi_carrier TYPE MpiCarrier INCLUDE include/yarp/os/impl/MpiCarrier.h)
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
  ADD_LIBRARY(mpi_carrier src/MpiCarrier.cpp src/MpiStream.cpp include/yarp/os/impl/MpiCarrier.h include/yarp/os/impl/MpiStream.h)
  TARGET_LINK_LIBRARIES(mpi_carrier ${MPI_LIBRARIES} ${YARP_LIBRARIES})

ENDIF (NOT SKIP_mpi_carrier)
