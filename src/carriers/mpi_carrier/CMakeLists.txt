# Copyright(C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Daniel Krieg
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(mpi_carrier
                       TYPE yarp::os::MpiP2PCarrier
                       INCLUDE include/yarp/os/MpiP2PCarrier.h)
  yarp_install(FILES mpi.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

cmake_dependent_option(MPI_DEBUG_MSG "Turn on status messages for MPI Carrier" OFF
                       "NOT SKIP_mpi_carrier OR NOT SKIP_mpibcast_carrier" OFF)
if(MPI_DEBUG_MSG)
  add_definitions(-DMPI_DEBUG)
endif(MPI_DEBUG_MSG)

if(NOT SKIP_mpi_carrier)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  include_directories(SYSTEM ${MPI_INCLUDE_PATH})

  yarp_add_plugin(yarp_mpi src/MpiCarrier.cpp
                           src/MpiComm.cpp
                           src/MpiStream.cpp
                           src/MpiP2PStream.cpp
                           include/yarp/os/MpiCarrier.h
                           include/yarp/os/MpiP2PCarrier.h
                           include/yarp/os/MpiStream.h
                           include/yarp/os/MpiP2PStream.h
                           include/yarp/os/MpiComm.h)
  target_link_libraries(yarp_mpi YARP_OS
                                 ${MPI_LIBRARIES})
  yarp_install(TARGETS yarp_mpi
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()


if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(mpibcast_carrier
                       TYPE yarp::os::MpiBcastCarrier
                       INCLUDE include/yarp/os/MpiBcastCarrier.h)
  yarp_install(FILES mpibcast.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_PLUGIN_LIBRARY)

if(NOT SKIP_mpibcast_carrier)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  include_directories(SYSTEM ${MPI_INCLUDE_PATH})

  yarp_add_plugin(yarp_mpibcast src/MpiCarrier.cpp
                                src/MpiBcastCarrier.cpp
                                src/MpiComm.cpp
                                src/MpiStream.cpp
                                src/MpiBcastStream.cpp
                                include/yarp/os/MpiCarrier.h
                                include/yarp/os/MpiBcastCarrier.h
                                include/yarp/os/MpiStream.h
                                include/yarp/os/MpiBcastStream.h
                                include/yarp/os/MpiComm.h)
  target_link_libraries(yarp_mpibcast YARP_OS
                                      ${MPI_LIBRARIES})
  yarp_install(TARGETS yarp_mpibcast
               COMPONENT runtime
               EXPORT YARP
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
