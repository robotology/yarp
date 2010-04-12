# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

add_subdirectory(libYARP_OS)

add_subdirectory(libYARP_sig)

add_subdirectory(libYARP_dev)

# Device related libraries / executables
option(CREATE_DEVICE_LIBRARY_MODULES "Compile device modules" FALSE)
if (CREATE_DEVICE_LIBRARY_MODULES)
  add_subdirectory(modules)
else (CREATE_DEVICE_LIBRARY_MODULES)
  add_subdirectory(yarpdev)
endif (CREATE_DEVICE_LIBRARY_MODULES)

add_subdirectory(yarpserver)

add_subdirectory(libYARP_math)

add_subdirectory(yarprun)

add_subdirectory(yarpview)

add_subdirectory(yarphear)

