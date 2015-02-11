# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_USE_PERSISTENT_NAMESERVER "Do you want a nameserver that stores its database in sqlite?" TRUE)
include(YarpInstallationHelpers)
yarp_configure_external_installation(yarp WITH_PLUGINS)

# libraries
add_subdirectory(libYARP_OS)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_dev)

set (YARP_LINK_PLUGINS TRUE)
if (BUILD_SHARED_LIBS)
  set (YARP_LINK_PLUGINS FALSE)
endif()

add_subdirectory(modules)

# We may have picked up some link directories - apply them here.
get_property(YARP_TREE_LINK_DIRS GLOBAL PROPERTY YARP_TREE_LINK_DIRS)
link_directories(${YARP_TREE_LINK_DIRS})

add_subdirectory(carriers)

add_subdirectory(idls)

add_subdirectory(libYARP_name)
add_subdirectory(libYARP_init)
add_subdirectory(libYARP_manager)
add_subdirectory(libYARP_logger)

add_subdirectory(libyarpc)
add_subdirectory(libyarpcxx)

# executables
add_subdirectory(yarpserver)
add_subdirectory(yarp)
add_subdirectory(yarprun)
add_subdirectory(yarphear)
add_subdirectory(yarpdev)
add_subdirectory(yarpmanager-console)
add_subdirectory(yarplogger-console)
add_subdirectory(yarpdatadumper)

# Qt5 GUIs
add_subdirectory(yarpview-qt)
add_subdirectory(yarpscope-qt)
add_subdirectory(yarpmanager-qt)
add_subdirectory(yarplogger-qt)
add_subdirectory(yarpdataplayer-qt)

# GTK2 GUIs
add_subdirectory(yarpview-gtk)
add_subdirectory(yarpscope-gtk)
add_subdirectory(yarpmanager-gtk)
add_subdirectory(yarpbuilder-gtk)
add_subdirectory(yarpdataplayer-gtk)

# data
add_subdirectory(yarpmanager)
add_subdirectory(yarpdataplayer)
