
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
SUBDIRS(libYARP_dev)
IF (CREATE_DEVICE_LIBRARY_MODULES)
   SUBDIRS(modules)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
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
  ENDIF (GtkPlus_FOUND)
# GtkMM GUIs have been moved to the iCub repository
#  IF (GtkMM_FOUND)
#      SUBDIRS(yarp-framegrabbergui)
##      SUBDIRS(yarp-motorcontrolgui)
#  ENDIF (GtkMM_FOUND)
ENDIF(CREATE_GUIS)

