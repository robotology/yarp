SUBDIRS(libYARP_OS libYARP_sig)

IF (COEXIST_WITH_YARP1 AND YARP1_FOUND)
   # optional libraries for coexisting with YARP1
   SUBDIRS(libYARP_OS_compat libYARP_sig_compat)
ENDIF (COEXIST_WITH_YARP1 AND YARP1_FOUND)

# option offered while device library build behavior is untested
SET(CREATE_DEVICE_LIBRARY FALSE CACHE BOOL "Do you want to compile the device library")

IF (CREATE_DEVICE_LIBRARY)
   SUBDIRS(libYARP_dev)
ENDIF (CREATE_DEVICE_LIBRARY)

