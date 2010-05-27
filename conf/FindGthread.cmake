# Tried to make variables uniform with cmake "standards"
# Now set Gthread_* variables (respect capitalization rule).
# Maintain backwards compatibiliy.
# Lorenzo Natale 14/05/2010
#
# On exit set:
# Gthread_FOUND
# Gthread_LIBRARIES
# Gthread_INCLUDE_DIRS
#
# Old variables:
#
# GTHREAD_FOUND
# GTHREAD_LINK_FLAGS
# GTHREAD_INCLUDE_DIR
#

IF(UNIX)
 INCLUDE(FindPkgConfig)
 if (PKG_CONFIG_FOUND)
   pkg_check_modules(GTHREAD "gthread-2.0")
   if (GTHREAD_FOUND)
     set(GTHREAD_INCLUDE_DIR ${GTHREAD_INCLUDE_DIRS})
     set(GTHREAD_LIBRARY_DIR ${GTHREAD_LIBRARY_DIRS})
     set(GTHREAD_LINK_FLAGS ${GTHREAD_LDFLAGS})
     set(GTHREAD_C_FLAGS ${GTHREAD_CFLAGS})
   endif (GTHREAD_FOUND)
 endif (PKG_CONFIG_FOUND)
 
 SET(GTHREAD_LINK_FLAGS "${GTHREAD_LINK_FLAGS}" CACHE INTERNAL "gthread link flags")
 SET(GTHREAD_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
 SET(GTHREAD_INCLUDE_DIR "${GTHREAD_INCLUDE_DIR}" CACHE INTERNAL "gthread include directory")

 
 SET(Gthread_LIBRARIES "${GTHREAD_LDFLAGS}" CACHE INTERNAL "gthread link flags")
 SET(Gthread_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
 SET(Gthread_INCLUDE_DIRS "${GTHREAD_INCLUDE_DIRS}" CACHE INTERNAL "gthread include directory")


 IF (GTHREAD_C_FLAGS)
 	SET(Gthread_FOUND TRUE)
 ELSE (GTHREAD_C_FLAGS)
	SET(Gthread_FOUND FALSE)
 ENDIF (GTHREAD_C_FLAGS)
 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gthread "GTHREAD not found" Gthread_LIBRARIES Gthread_INCLUDE_DIRS)


ELSE (UNIX)
  FIND_LIBRARY(GTK_thread_lib
	NAMES gthread-2.0
	PATHS $ENV{GTK_BASEPATH}/lib)
  
  SET(Gthread_INCLUDE_DIRS "")
  
  IF (GTK_thread_lib)
    SET(Gthread_FOUND TRUE)
    SET(Gthread_LIBRARIES
	  ${GTK_thread_lib})
  ENDIF(GTK_thread_lib)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gthread "GTHREAD not found" Gthread_LIBRARIES)

ENDIF (UNIX)



