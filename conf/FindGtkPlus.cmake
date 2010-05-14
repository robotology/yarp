# Tried to make variables uniform with cmake "standards"
# Now set GtkPlus_* variables (respect capitalization rule).
# Maintain backwards compatibiliy.
# Lorenzo Natale 14/05/2010
#
# On exit set:
# GtkPlus_FOUND
# GtkPlus_LIBRARIES
# GtkPlus_INCLUDE_DIRS
#
# Old variables:
#
# GTKPLUS_FOUND
# GTKPLUS_LIBRARY_DIR 
# GTKPLUS_LINK_FLAGS
# GTKPLUS_C_FLAGS
# GTKPLUS_INCLUDE_DIR
#


IF(UNIX)
  INCLUDE(FindPkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(GTKPLUS "gtk+-2.0")
    if (GTKPLUS_FOUND)
      set(GTKPLUS_INCLUDE_DIR ${GTKPLUS_INCLUDE_DIRS})
      set(GTKPLUS_LIBRARY_DIR ${GTKPLUS_LIBRARY_DIRS})
      set(GTKPLUS_LINK_FLAGS ${GTKPLUS_LDFLAGS})
      set(GTKPLUS_C_FLAGS ${GTKPLUS_CFLAGS})
    endif (GTKPLUS_FOUND)
  endif (PKG_CONFIG_FOUND)

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
 SET(GTKPLUS_LIBRARY_DIR "${GTKPLUS_LIBRARY_DIR}" CACHE INTERNAL "gtk+ library directory")

 SET(GtkPlus_LIBRARIES "${GTKPLUS_LINK_FLAGS}" CACHE INTERNAL "gtk+ link flags")
 SET(GtkPlus_INCLUDE_DIRS "${GTKPLUS_INCLUDE_DIR}" CACHE INTERNAL "gtk+ include directory")

 # message(STATUS "Libraries --> ${GtkPlus_LIBRARIES}")
 # message(STATUS "IncludeDirs --> ${GtkPlus_INCLUDE_DIRS}")
 # message(STATUS "LinkFlags --> ${GTKPLUS_LINK_DIR}")
 # message(STATUS "CFlags --> ${GTKPLUS_C_FLAGS}")
 
 #ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
 #LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})

 SET(GtkPlus_LIBRARIES "${GTKPLUS_LINK_FLAGS}" CACHE STRING "gtk+ link flags")
 SET(GtkPlus_INCLUDE_DIRS "${GTKPLUS_INCLUDE_DIR}" CACHE STRING "gtk+ include directory")


 IF (GTKPLUS_C_FLAGS)
 	SET(GtkPlus_FOUND TRUE)
 ELSE (GTKPLUS_C_FLAGS)
	SET(GtkPlus_FOUND FALSE)
 ENDIF (GTKPLUS_C_FLAGS)
ELSE (UNIX)
 	FIND_PACKAGE(GtkWin32) 
 	
 	SET(GtkPlus_LIBRARIES "${GTKPLUS_LINK_FLAGS}" CACHE STRING "gtk+ link flags")
   SET(GtkPlus_INCLUDE_DIRS "${GTKPLUS_INCLUDE_DIR}" CACHE STRING "gtk+ include directory")

	if (GTK_FOUND)
		set(GtkPlus_FOUND TRUE)
		set(GtkPlus_LIBRARIES "${GTK_LIBRARIES}" CACHE STRING "gtk+ link flags")
   	set(GtkPlus_INCLUDE_DIRS "${GTK_INCLUDE_DIR}" CACHE STRING "gtk+ include directory")
	endif (GTK_FOUND)
	
ENDIF (UNIX)



