# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_USE_PERSISTENT_NAMESERVER "Do you want a nameserver that stores its database in sqlite?" TRUE)
include(YarpInstallationHelpers)
yarp_configure_external_installation(yarp WITH_PLUGINS)

# public libraries
add_subdirectory(libYARP_OS)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_dev)

# plugins
set (YARP_LINK_PLUGINS TRUE)
if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
  set (YARP_LINK_PLUGINS FALSE)
endif()
add_subdirectory(modules)
add_subdirectory(carriers)
add_subdirectory(libYARP_init)

# private libraries
add_subdirectory(libYARP_name)
add_subdirectory(libYARP_serversql)
add_subdirectory(libYARP_manager)
add_subdirectory(libYARP_logger)

if(YARP_COMPILE_EXECUTABLES)
    # idl compilers (thrift and ros)
    add_subdirectory(idls)

    # executables
    add_subdirectory(yarpserver)
    add_subdirectory(yarp)
    add_subdirectory(yarprun)
    add_subdirectory(yarphear)
    add_subdirectory(yarpdev)
    add_subdirectory(yarprobotinterface)
    add_subdirectory(yarpmanager-console)
    add_subdirectory(yarplogger-console)
    add_subdirectory(yarpdatadumper)

    # Qt5 GUIs
    add_subdirectory(yarpview-qt)
    add_subdirectory(yarpscope-qt)
    add_subdirectory(yarpmanager-qt)
    add_subdirectory(yarplogger-qt)
    add_subdirectory(yarpdataplayer-qt)
    add_subdirectory(yarpmotorgui-qt)
    add_subdirectory(yarpbatterygui-qt)
    add_subdirectory(yarplaserscannergui)
    add_subdirectory(yarpmanager++)

    # GTK2 GUIs
    add_subdirectory(yarpbuilder-gtk)
    if(NOT YARP_NO_DEPRECATED)
        add_subdirectory(yarpview-gtk)
        add_subdirectory(yarpscope-gtk)
        add_subdirectory(yarpmanager-gtk)
        add_subdirectory(yarpmotorgui-gtk)
        add_subdirectory(yarpdataplayer-gtk)
    endif()
endif()


# unmaintained stuff
if(YARP_COMPILE_UNMAINTAINED)
    add_subdirectory(libyarpc)
    add_subdirectory(libyarpcxx)
    if(YARP_COMPILE_EXECUTABLES)
        add_subdirectory(yarpfs)
    endif()
endif()
