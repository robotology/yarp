# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# libraries
add_subdirectory(libYARP_OS)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_dev)
add_subdirectory(modules)

# We may have picked up some link directories - apply them here.
get_property(YARP_TREE_LINK_DIRS GLOBAL PROPERTY YARP_TREE_LINK_DIRS)
link_directories(${YARP_TREE_LINK_DIRS})

add_subdirectory(carriers)

add_subdirectory(libYARP_name)
add_subdirectory(libYARP_init)
add_subdirectory(libyarpc)
add_subdirectory(libyarpcxx)

# executables
add_subdirectory(yarp)
add_subdirectory(yarpserver)
add_subdirectory(yarprun)
add_subdirectory(yarpview)
add_subdirectory(yarphear)
add_subdirectory(yarpdev)
add_subdirectory(yarpmanager)

#tests
add_subdirectory(tests)
