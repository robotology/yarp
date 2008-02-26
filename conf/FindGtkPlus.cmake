IF(UNIX)
 IF(NOT PKGCONFIG_EXECUTABLE) 
   INCLUDE(UsePkgConfig)
 ENDIF(NOT PKGCONFIG_EXECUTABLE)
 IF(PKGCONFIG_EXECUTABLE)
	PKGCONFIG("gtk+-2.0" GTKPLUS_INCLUDE_DIR GTKPLUS_LIBRARY_DIR GTKPLUS_LINK_FLAGS GTKPLUS_C_FLAGS)
 ENDIF(PKGCONFIG_EXECUTABLE)


#IF (APPLE)
	# there is a problem with some versions of pkg-config introducing
	# newline characters that don't get cleaned up in some versions
	# of cmake.  No time to fix this now...
	# the same problem seems to happen also on Linux/Debian (ask Giorgio Metta).
#The following IF appends /usr/lib as a link flag, this
#asks the linker to link a directory, which is wrong and causes a link error.
#It looks more correct to remove the command completely. Another option is 
#to add -L${GTKPLUS_LIBRARY_DIR} instead of ${GTKPLUS_LIBRARY_DIR}
#
#	IF (GTKPLUS_LIBRARY_DIR)
#	SET(GTKPLUS_LINK_FLAGS "${GTKPLUS_LINK_FLAGS} ${GTKPLUS_LIBRARY_DIR}")
#	ENDIF (GTKPLUS_LIBRARY_DIR)

#ENDIF (APPLE)

 SET(GTKPLUS_LINK_FLAGS "${GTKPLUS_LINK_FLAGS}" CACHE INTERNAL "gtk+ link flags")
 SET(GTKPLUS_C_FLAGS "${GTKPLUS_C_FLAGS}" CACHE INTERNAL "gtk+ include flags")
 SET(GTKPLUS_INCLUDE_DIR "${GTKPLUS_INCLUDE_DIR}" CACHE INTERNAL "gtk+ include directory")

 #ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
 #LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})

 IF (GTKPLUS_C_FLAGS)
 	SET(GtkPlus_FOUND TRUE)
 ELSE (GTKPLUS_C_FLAGS)
	SET(GtkPlus_FOUND FALSE)
 ENDIF (GTKPLUS_C_FLAGS)
ELSE (UNIX)
 	FIND_PACKAGE(GtkWin32) 
ENDIF (UNIX)
