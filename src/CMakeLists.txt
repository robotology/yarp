SUBDIRS(libYARP_OS libYARP_OS_compat libYARP_sig)

IF (COEXIST_WITH_YARP1 AND YARP1_FOUND)
   # optional libraries for coexisting with YARP1
   SUBDIRS(libYARP_sig_compat libYARP_dev)
ENDIF (COEXIST_WITH_YARP1 AND YARP1_FOUND)
