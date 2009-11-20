# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# pick up any generated header files
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)

SUBDIRS(libYARP_OS libYARP_sig)

SET(CREATE_YARPSERVER3 FALSE CACHE BOOL "Do you want to compile yarpserver3 (experimental)")

IF (CREATE_YARPSERVER3)
 SUBDIRS(yarpserver)
ENDIF (CREATE_YARPSERVER3)

IF (YARP_HAS_MATH_LIB)
 SUBDIRS(libYARP_math)
ENDIF(YARP_HAS_MATH_LIB)

# Device related libraries / executables
IF (MERGE_DEVICE_LIBRARY_MODULES AND CREATE_DEVICE_LIBRARY_MODULES)
  # libYARP_dev and yarpmod need to be created, and would have
  # a circular dependency if compiled separately.
  # Solution: empty out libYARP_dev and put it in yarpmod
  MESSAGE(STATUS "Mergible")
  SET(YARP_MERGING TRUE)
  SUBDIRS(libYARP_dev)
  SUBDIRS(modules)
  SET(YARP_MERGING FALSE)
ELSE (MERGE_DEVICE_LIBRARY_MODULES AND CREATE_DEVICE_LIBRARY_MODULES)
  SUBDIRS(libYARP_dev)
  IF (CREATE_DEVICE_LIBRARY_MODULES)
     SUBDIRS(modules)
  ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
ENDIF (MERGE_DEVICE_LIBRARY_MODULES AND CREATE_DEVICE_LIBRARY_MODULES)
SUBDIRS(yarpdev)
SUBDIRS(yarprun)

IF (CREATE_GUIS)
  IF (GtkPlus_FOUND)
	IF (COMPILE_NEW_YARPVIEW)
	  IF (Gthread_FOUND)
		SUBDIRS(yarpview)
	  ENDIF(Gthread_FOUND)
	ELSE(COMPILE_NEW_YARPVIEW)
      SUBDIRS(yarpview_old)
	ENDIF(COMPILE_NEW_YARPVIEW)
  ELSE (GtkPlus_FOUND)
      MESSAGE(ERROR " Gtk not found ")
  ENDIF (GtkPlus_FOUND)
# GtkMM GUIs have been moved to the iCub repository
#  IF (GtkMM_FOUND)
#      SUBDIRS(yarp-framegrabbergui)
##      SUBDIRS(yarp-motorcontrolgui)
#  ENDIF (GtkMM_FOUND)
ENDIF(CREATE_GUIS)

