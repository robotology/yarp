SUBDIRS(libYARP_OS libYARP_sig)

IF (COEXIST_WITH_YARP1 AND YARP1_FOUND)
   # optional libraries for coexisting with YARP1
   SUBDIRS(libYARP_OS_compat libYARP_sig_compat)
ENDIF (COEXIST_WITH_YARP1 AND YARP1_FOUND)


IF (CREATE_DEVICE_LIBRARY)
   SUBDIRS(libYARP_dev)
   SUBDIRS(yarpdev)
ENDIF (CREATE_DEVICE_LIBRARY)

IF (CREATE_GUIS)
  IF (GtkPlus_FOUND)
      SUBDIRS(yarp-imageviewer)
  ENDIF (GtkPlus_FOUND)
  IF (GtkMM_FOUND)
  #    SUBDIRS(yarp-framegrabbergui)
  ENDIF (GtkMM_FOUND)
ENDIF(CREATE_GUIS)

