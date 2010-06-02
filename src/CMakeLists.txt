# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# libraries
add_subdirectory(libYARP_OS)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_dev)
add_subdirectory(modules)
add_subdirectory(carriers)
add_subdirectory(libYARP_init)

# executables
add_subdirectory(yarp)
add_subdirectory(yarpserver)
add_subdirectory(yarprun)
add_subdirectory(yarpview)
add_subdirectory(yarphear)
add_subdirectory(yarpdev)

#tests
add_subdirectory(tests)
