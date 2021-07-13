# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(mpi
  CATEGORY carrier
  TYPE MpiP2PCarrier
  INCLUDE MpiP2PCarrier.h
  EXTRA_CONFIG
    CODE="MPI_____"
  DEPENDS "YARP_HAS_MPI"
)

include(YarpDeprecatedOption)
yarp_deprecated_option(MPI_DEBUG_MSG) # Since YARP 3.3
yarp_deprecated_option(YARP_MPI_DEBUG_MSG) # Since YARP 3.4

if(NOT SKIP_mpi)
  yarp_add_plugin(yarp_mpi)

  target_sources(yarp_mpi
    PRIVATE
      MpiCarrier.cpp
      MpiComm.cpp
      MpiStream.cpp
      MpiP2PStream.cpp
      MpiLogComponent.cpp
      MpiCarrier.h
      MpiP2PCarrier.h
      MpiStream.h
      MpiP2PStream.h
      MpiComm.h
      MpiLogComponent.h
  )

  target_link_libraries(yarp_mpi PRIVATE YARP::YARP_os)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  target_include_directories(yarp_mpi SYSTEM PRIVATE ${MPI_INCLUDE_PATH})
  target_link_libraries(yarp_mpi PRIVATE ${MPI_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS MPI) (not using targets)

  yarp_install(
    TARGETS yarp_mpi
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mpi PROPERTY FOLDER "Plugins/Carrier")
endif()


yarp_prepare_plugin(mpibcast
  CATEGORY carrier
  TYPE MpiBcastCarrier
  INCLUDE MpiBcastCarrier.h
  EXTRA_CONFIG
    CODE="MPIBCAST"
  DEPENDS "YARP_HAS_MPI"
)

if(NOT SKIP_mpibcast)
  yarp_add_plugin(yarp_mpibcast)

  target_sources(yarp_mpibcast
    PRIVATE
      MpiCarrier.cpp
      MpiBcastCarrier.cpp
      MpiComm.cpp
      MpiStream.cpp
      MpiBcastStream.cpp
      MpiLogComponent.cpp
      MpiCarrier.h
      MpiBcastCarrier.h
      MpiStream.h
      MpiBcastStream.h
      MpiComm.h
      MpiLogComponent.h
  )

  target_link_libraries(yarp_mpibcast PRIVATE YARP::YARP_os)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  target_include_directories(yarp_mpibcast SYSTEM PRIVATE ${MPI_INCLUDE_PATH})
  target_link_libraries(yarp_mpibcast PRIVATE ${MPI_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS MPI) (not using targets)

  yarp_install(
    TARGETS yarp_mpibcast
    COMPONENT ${YARP_PLUGIN_MASTER}
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mpibcast PROPERTY FOLDER "Plugins/Carrier")

endif()
