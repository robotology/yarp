# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# public libraries
add_subdirectory(libYARP_conf)
add_subdirectory(libYARP_os)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_eigen)
add_subdirectory(libYARP_gsl)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_dev)
add_subdirectory(libYARP_pcl)
add_subdirectory(libYARP_cv)
add_subdirectory(libYARP_robotinterface)

# private libraries
add_subdirectory(libYARP_companion)
add_subdirectory(libYARP_name)
add_subdirectory(libYARP_serversql)
add_subdirectory(libYARP_run)
add_subdirectory(libYARP_manager)
add_subdirectory(libYARP_profiler)
add_subdirectory(libYARP_logger)
add_subdirectory(libYARP_dataplayer)
add_subdirectory(libYARP_robottestingframework)

# plugins
add_subdirectory(carriers)
add_subdirectory(portmonitors)
add_subdirectory(devices)
add_subdirectory(libYARP_init)

if(YARP_COMPILE_EXECUTABLES)
    # idl compilers (thrift and ros)
    add_subdirectory(yarpidl_thrift)
    if(NOT CREATE_SHARED_LIBS)
        include(YarpInstallBasicPackageFiles)
        yarp_install_basic_package_files(
          YARP_idl_tools
          INCLUDE_CONTENT "include(\"\\\${YARP_MODULE_DIR}/YarpIDL.cmake\")"
        )
    endif()

    # executables
    add_subdirectory(yarpserver)
    add_subdirectory(yarp)
    add_subdirectory(yarp-config)
    add_subdirectory(yarprun)
    add_subdirectory(yarphear)
    add_subdirectory(yarpdev)
    add_subdirectory(yarprobotinterface)
    add_subdirectory(yarpconnectionsinfo)
    add_subdirectory(yarpmanager-console)
    add_subdirectory(yarplogger-console)
    add_subdirectory(yarpdataplayer-console)
    add_subdirectory(yarpdatadumper)
    add_subdirectory(yarpActionsPlayer)

    # GUIs
    add_subdirectory(guis)

    #other
    add_subdirectory(yarpgstreamerplugins)
    add_subdirectory(yarpDeviceParamParserGenerator)
endif()

# Robot Testing Framework addons
if(YARP_COMPILE_RobotTestingFramework_ADDONS)
  add_subdirectory(robottestingframework-plugins)
endif()
