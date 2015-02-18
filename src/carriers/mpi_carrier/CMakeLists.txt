# Copyright(C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Daniel Krieg
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(mpi_carrier TYPE yarp::os::MpiP2PCarrier INCLUDE include/yarp/os/MpiP2PCarrier.h)
  yarp_install(FILES mpi.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_PLUGIN_LIBRARY)

if(NOT SKIP_mpi_carrier)

  find_package(MPI REQUIRED)
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})
  add_definitions(-DCREATE_MPI_CARRIER)

  # include print statements for debugging purpose
  option(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF)
  if(MPI_DEBUG_MSG)
    add_definitions(-DMPI_DEBUG)
  endif(MPI_DEBUG_MSG)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  yarp_add_plugin(yarp_mpi src/MpiCarrier.cpp
                           src/MpiComm.cpp
                           src/MpiStream.cpp
                           src/MpiP2PStream.cpp
                           include/yarp/os/MpiCarrier.h
                           include/yarp/os/MpiP2PCarrier.h
                           include/yarp/os/MpiStream.h
                           include/yarp/os/MpiP2PStream.h
                           include/yarp/os/MpiComm.h
  )
  target_link_libraries(yarp_mpi YARP_OS YARP_sig)
  target_link_libraries(yarp_mpi ${ACE_LIBRARIES})
  target_link_libraries(yarp_mpi ${MPI_LIBRARIES})

endif(NOT SKIP_mpi_carrier)


if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(mpibcast_carrier TYPE yarp::os::MpiBcastCarrier INCLUDE include/yarp/os/MpiBcastCarrier.h)
endif(COMPILE_PLUGIN_LIBRARY)

if(NOT SKIP_mpibcast_carrier)

  find_package(MPI REQUIRED)
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})
  add_definitions(-DCREATE_MPI_CARRIER)

  # include print statements for debugging purpose
  option(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF)
  if(MPI_DEBUG_MSG)
    add_definitions(-DMPI_DEBUG)
  endif(MPI_DEBUG_MSG)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  yarp_add_plugin(yarp_mpibcast src/MpiCarrier.cpp
                                src/MpiBcastCarrier.cpp
                                src/MpiComm.cpp
                                src/MpiStream.cpp
                                src/MpiBcastStream.cpp
                                include/yarp/os/MpiCarrier.h
                                include/yarp/os/MpiBcastCarrier.h
                                include/yarp/os/MpiStream.h
                                include/yarp/os/MpiBcastStream.h
                                include/yarp/os/MpiComm.h
  )
  target_link_libraries(yarp_mpibcast YARP_OS YARP_sig)
  target_link_libraries(yarp_mpibcast ${ACE_LIBRARIES})
  target_link_libraries(yarp_mpibcast ${MPI_LIBRARIES})

endif(NOT SKIP_mpibcast_carrier)
