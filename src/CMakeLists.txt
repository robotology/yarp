SUBDIRS(libYARP_OS libYARP_sig)

IF (COEXIST_WITH_YARP1 AND YARP1_FOUND)
   # optional libraries for coexisting with YARP1
   SUBDIRS(libYARP_OS_compat libYARP_sig_compat)
ENDIF (COEXIST_WITH_YARP1 AND YARP1_FOUND)

IF (YARP_HAS_MATH_LIB)
 SUBDIRS(libYARP_math)
ENDIF(YARP_HAS_MATH_LIB)

# Device related libraries / executables
SUBDIRS(libYARP_dev)
IF (CREATE_DEVICE_LIBRARY_MODULES)
   SUBDIRS(modules)
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)
SUBDIRS(yarpdev)

IF (CREATE_GUIS)
  IF (GtkPlus_FOUND)
      SUBDIRS(yarp-imageviewer)
  ENDIF (GtkPlus_FOUND)
# GtkMM GUIs have been moved to the iCub repository
#  IF (GtkMM_FOUND)
#      SUBDIRS(yarp-framegrabbergui)
##      SUBDIRS(yarp-motorcontrolgui)
#  ENDIF (GtkMM_FOUND)
ENDIF(CREATE_GUIS)

