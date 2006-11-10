# 
# Try to find OpenCV library
# Once run this will define: 
# 
# OPENCV_FOUND
# OPENCV_INCLUDE_DIR
# OPENCV_LIBRARIES
# OPENCV_LINK_DIRECTORIES
##
# deprecated:
# (JW)-- OPENCV_EXE_LINKER_FLAGS
# 
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006 complete rewrite by Jan Woetzel
##
# www.mip.informatik.uni-kiel.de/
# --------------------------------
#
# Modified by nat for YARP
# - nat modified to work on win: added "$ENV{OPENCV_DIR}/otherlibs/highgui/include"
# - nat, 19-oct 06: cxcore does not seem required by the opencv pakage that is 
# distributed with Debian stable. Still a requirement in windows
# - nat, 20-oct 06: not smart enough to handle possible clashes between 
# different versions of opencv if installed in the system. Be careful.
# - nat, 23-oct 06: First check OPENCV_DIR, OPENCV_ROOT or OPENCV_HOME
# - paulfitz, 5-nov 06: made link with libhighgui be default true
#   so that the opencv driver can be compiled. Nat, can you explain again 
#   which systems don't have this?

SET(LINK_LIB_HIGHGUI TRUE CACHE BOOL "Do you want to link against libhighgui?")
IF (LINK_LIB_HIGHGUI AND WIN32)
  SET(LINK_LIB_CVCAM)
ENDIF(LINK_LIB_HIGHGUI AND WIN32)

SET(IS_GNUCXX3 FALSE)
SET(IS_GNUCXX4 FALSE)
IF    (${CMAKE_COMPILER_IS_GNUCXX})

  MESSAGE(STATUS "Checking GNUCXX version 3/4 to determine  OpenCV /opt/net/ path")
  EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CXX_COMPILER_VERSION)
  
  IF   (CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")
    #   MESSAGE("DBG OpenCV for 3.x")
    SET(IS_GNUCXX3 TRUE)
    # ELSE (CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")
    #   MESSAGE("DBG not 3.x")
  ENDIF(CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")

  IF   (CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
    #   MESSAGE("DBG OpenCV for 4.x")
    SET(IS_GNUCXX4 TRUE)
    # ELSE (CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
    #   MESSAGE("DBG not 4.x")
  ENDIF(CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")

ENDIF (${CMAKE_COMPILER_IS_GNUCXX})

# CHECK OPENCV_ROOT
IF (EXISTS "$ENV{OPENCV_ROOT}")
SET(OPENCV_POSSIBLE_INCDIRS
  "$ENV{OPENCV_ROOT}"
  "$ENV{OPENCV_ROOT}/include"
  "$ENV{OPENCV_ROOT}/include/cv" 
  "$ENV{OPENCV_ROOT}/include/opencv" 
  "$ENV{OPENCV_ROOT}/cxcore/include"
  "$ENV{OPENCV_ROOT}/cv/include"
  "$ENV{OPENCV_ROOT}/cvaux/include"
  "$ENV{OPENCV_ROOT}/otherlibs/cvcam/include"
  "$ENV{OPENCV_ROOT}/otherlibs/highgui/include"
  "$ENV{OPENCV_ROOT}/otherlibs/highgui/")

SET(OPENCV_POSSIBLE_LIBRARY_PATHS
  "$ENV{OPENCV_ROOT}/lib"
  "$ENV{OPENCV_ROOT}")
ENDIF (EXISTS "$ENV{OPENCV_ROOT}")

# CHECK OPENCV_DIR
IF (EXISTS "$ENV{OPENCV_DIR}")
SET(OPENCV_POSSIBLE_INCDIRS
  "$ENV{OPENCV_DIR}"
  "$ENV{OPENCV_DIR}/include"
  "$ENV{OPENCV_DIR}/include/cv" 
  "$ENV{OPENCV_DIR}/include/opencv" 
  "$ENV{OPENCV_DIR}/cxcore/include"
  "$ENV{OPENCV_DIR}/cv/include"
  "$ENV{OPENCV_DIR}/cvaux/include"
  "$ENV{OPENCV_DIR}/otherlibs/cvcam/include"
  "$ENV{OPENCV_DIR}/otherlibs/highgui/include"
  "$ENV{OPENCV_DIR}/otherlibs/highgui/")

SET(OPENCV_POSSIBLE_LIBRARY_PATHS
  "$ENV{OPENCV_DIR}"
  "$ENV{OPENCV_DIR}/lib")

ENDIF(EXISTS "$ENV{OPENCV_DIR}")

# CHECK OPENCV_HOME
IF (EXISTS "$ENV{OPENCV_HOME}")
SET(OPENCV_POSSIBLE_INCDIRS
  "$ENV{OPENCV_HOME}"
  "$ENV{OPENCV_HOME}/include"
  "$ENV{OPENCV_HOME}/include/cv"
  "$ENV{OPENCV_HOME}/include/opencv")
ENDIF (EXISTS "$ENV{OPENCV_HOME}")

IF (NOT OPENCV_POSSIBLE_INCDIRS)
SET(OPENCV_POSSIBLE_INCDIRS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]"  
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]/include"
  "$ENV{ProgramFiles}/OpenCV"
  "$ENV{ProgramFiles}/OpenCV/include"
  "$ENV{ProgramFiles}/OpenCV/cxcore/include"
  "$ENV{ProgramFiles}/OpenCV/cv/include"
  "$ENV{ProgramFiles}/OpenCV/cvaux/include"
  "$ENV{ProgramFiles}/OpenCV/otherlibs/cvcam/include"
  "$ENV{ProgramFiles}/OpenCV/otherlibs/highgui/include"
  "$ENV{ProgramFiles}/OpenCV/otherlibs/highgui"
  /usr/include/opencv
  /usr/local/include/opencv)
ENDIF (NOT OPENCV_POSSIBLE_INCDIRS)

IF (NOT OPENCV_POSSIBLE_LIBRARY_PATHS)
SET(OPENCV_POSSIBLE_LIBRARY_PATHS
  "$ENV{ProgramFiles}/OpenCV/lib"
  "/usr/local/lib"
  "/usr/lib"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]/lib")
ENDIF(NOT OPENCV_POSSIBLE_LIBRARY_PATHS)

IF   (IS_GNUCXX3)
  SET(OPENCV_POSSIBLE_INCDIRS ${OPENCV_POSSIBLE_INCDIRS} 
    /opt/net/gcc33/OpenCV/
    /opt/net/gcc33/OpenCV/include
    /opt/net/gcc33/OpenCV/include/opencv )
ENDIF(IS_GNUCXX3)
IF   (IS_GNUCXX4)
  SET(OPENCV_POSSIBLE_INCDIRS ${OPENCV_POSSIBLE_INCDIRS} 
    /opt/net/gcc41/OpenCV/
    /opt/net/gcc41/OpenCV/include
    /opt/net/gcc41/OpenCV/include/opencv )
ENDIF(IS_GNUCXX4)
#MESSAGE("DBG (OPENCV_POSSIBLE_INCDIRS=${OPENCV_POSSIBLE_INCDIRS}")

# candidates for OpenCV library directories:

IF   (IS_GNUCXX3)
  SET(OPENCV_POSSIBLE_LIBRARY_PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS}
    /opt/net/gcc33/OpenCV
    /opt/net/gcc33/OpenCV/lib )
ENDIF(IS_GNUCXX3)
IF   (IS_GNUCXX4)
  SET(OPENCV_POSSIBLE_LIBRARY_PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS}
    /opt/net/gcc41/OpenCV
    /opt/net/gcc41/OpenCV/lib
)
ENDIF(IS_GNUCXX4)

#MESSAGE("DBG (OPENCV_POSSIBLE_LIBRARY_PATHS=${OPENCV_POSSIBLE_LIBRARY_PATHS}")

# find (all) header files for include directories:
FIND_PATH(OPENCV_INCLUDE_DIR_CXCORE   cxcore.h  ${OPENCV_POSSIBLE_INCDIRS} )
FIND_PATH(OPENCV_INCLUDE_DIR_CV       cv.h      ${OPENCV_POSSIBLE_INCDIRS} )
FIND_PATH(OPENCV_INCLUDE_DIR_CVAUX    cvaux.h   ${OPENCV_POSSIBLE_INCDIRS} )
FIND_PATH(OPENCV_INCLUDE_DIR_HIGHGUI  highgui.h ${OPENCV_POSSIBLE_INCDIRS} )
FIND_PATH(OPENCV_INCLUDE_DIR_CVCAM    cvcam.h   ${OPENCV_POSSIBLE_INCDIRS} )

#MESSAGE("DBG OPENCV_INCLUDE_DIR_CV=${OPENCV_INCLUDE_DIR_CV} ")

# find (all) libraries - some dont exist on Linux
FIND_LIBRARY(OPENCV_LIBRARY
  NAMES opencv cv cv0.9 cv0.9.5 cv0.9.6 cv0.9.7 cv0.9.8 cv0.9.9
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS}
  DOC "Location of the opencv lib")

FIND_LIBRARY(OPENCV_CVAUX_LIBRARY
  NAMES cvaux cvaux0.9 cvaux0.9.5 cvaux0.9.6 cvaux0.9.7 cvaux0.9.8 cvaux0.9.9
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} 
  DOC "Location of the cvaux lib")

FIND_LIBRARY(OPENCV_CXCORE_LIBRARY
  NAMES cxcore cxcore0.9  cxcore0.9.5 cxcore0.9.6 cxcore0.9.7 cxcore0.9.8 cxcore0.9.9
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} 
  DOC "Location of the cvcore lib")

FIND_LIBRARY(OPENCV_HIGHGUI_LIBRARY
 NAMES highgui highgui0.9 highgui0.9.5 highgui0.9.6 highgui0.9.7 highgui0.9.8 highgui0.9.9
 PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} 
 DOC "Location of the highgui library")
  
# optional CVCAM libs (WIN32 only)
FIND_LIBRARY(OPENCV_CVCAM_LIBRARY
  NAMES cvcam
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} 
  DOC "Location of the cvcam lib") 

SET(OPENCV_FOUND ON)
###########################################
## CHECK HEADER FILES

# REQUIRED LIBS
FOREACH(INCDIR 
  OPENCV_INCLUDE_DIR_CV 
  OPENCV_INCLUDE_DIR_CVAUX)
  IF    (${INCDIR})
    SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${${INCDIR}} )
  ELSE  (${INCDIR})
	MESSAGE("- DBG OPENCV_INCLUDE_DIR_CVCAM=${OPENCV_INCLUDE_DIR_CVCAM}")
    SET(OPENCV_FOUND OFF)
  ENDIF (${INCDIR})  
ENDFOREACH(INCDIR)

# some people write opencv/foo.h, some write foo.h
IF(OPENCV_INCLUDE_DIR_CV)   
  SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} "${OPENCV_INCLUDE_DIR_CV}/..")
ENDIF(OPENCV_INCLUDE_DIR_CV)


# CVCAM exists only on Windows (check this -- nat)
IF (LINK_LIB_CVCAM)
  IF(OPENCV_INCLUDE_DIR_CVCAM)
	SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_CVCAM} )
  ELSE (OPENCV_INCLUDE_DIR_CVCAM)
	# exists only on Windows, thus only there required
	IF (WIN32)
      SET(OPENCV_FOUND OFF)
	  MESSAGE("- DBG OPENCV_INCLUDE_DIR_CVCAM=${OPENCV_INCLUDE_DIR_CVCAM} ")
	ENDIF (WIN32)
  ENDIF(OPENCV_INCLUDE_DIR_CVCAM)
ENDIF(LINK_LIB_CVCAM)

# MESSAGE("DBG OPENCV_INCLUDE_DIR=${OPENCV_INCLUDE_DIR}")

# libcxcore does not seem to be always required (different distribution behave 
# differently)
IF (OPENCV_INCLUDE_DIR_CXCORE)
  SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_CXCORE})
ELSE (OPENCV_INCLUDE_DIR_CXCORE)
  IF (WIN32) #required in win
	SET(OPENCV_FOUND OFF)
	MESSAGE("- DBG OPENCV_INCLUDE_DIR_CXCORE=${OPENCV_INCLUDE_DIR_CXCORE} ")
  ENDIF (WIN32)
ENDIF(OPENCV_INCLUDE_DIR_CXCORE)

IF(LINK_LIB_HIGHGUI)
  IF (OPENCV_INCLUDE_DIR_HIGHGUI)
	SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_HIGHGUI})
  ELSE (OPENCV_INCLUDE_DIR_HIGHGUI)
	IF (WIN32)
	  SET(OPENCV_FOUND OFF)
	MESSAGE("- DBG OPENCV_INCLUDE_DIR_HIGHGUI=${OPENCV_INCLUDE_DIR_HIGHGUI} ")
	ENDIF (WIN32)
  ENDIF(OPENCV_INCLUDE_DIR_HIGHGUI)
ENDIF(LINK_LIB_HIGHGUI)

#################################
## LIBRARIES

# REQUIRED LIBRARIES
FOREACH(LIBNAME  
  OPENCV_LIBRARY 
  OPENCV_CVAUX_LIBRARY)
IF (${LIBNAME})
    SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${${LIBNAME}} )
  ELSE  (${LIBNAME})
    MESSAGE("${LIBNAME} not found turning off OPENCV_FOUND")
    SET(OPENCV_FOUND OFF)
  ENDIF (${LIBNAME})
ENDFOREACH(LIBNAME)

IF (OPENCV_CXCORE_LIBRARY)
  SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${OPENCV_CXCORE_LIBRARY})
ELSE (OPENCV_CXCORE_LIBRARY)
  IF (WIN32) #this is required on windows
	SET(OPENCV_FOUND OFF)
	MESSAGE("OPENCV_CXCORE_LIBRARY not found turning off OPENCV_FOUND")
  ENDIF (WIN32)
ENDIF(OPENCV_CXCORE_LIBRARY)

# CVCAM exists only on Windows (check this -- nat)
IF(LINK_LIB_CVCAM)
  IF (OPENCV_CVCAM_LIBRARY)
	SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${OPENCV_CVCAM_LIBRARY} )
  ELSE  (OPENCV_CVCAM_LIBRARY)
	IF (WIN32)
      SET(OPENCV_FOUND OFF)
	  MESSAGE("OPENCV_CVCAM_LIBRARY not found turning off OPENCV_FOUND")
	ENDIF (WIN32)
  ENDIF (OPENCV_CVCAM_LIBRARY)
ENDIF(LINK_LIB_CVCAM)
# MESSAGE("DBG OPENCV_LIBRARIES=${OPENCV_LIBRARIES}")

IF(LINK_LIB_HIGHGUI)
  IF (OPENCV_HIGHGUI_LIBRARY)
	SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${OPENCV_HIGHGUI_LIBRARY})
  ELSE (OPENCV_HIGHGUI_LIBRARY)
	IF (WIN32) #this is required on windows
	  SET(OPENCV_FOUND OFF)
	  MESSAGE("OPENCV_HIGHGUI_LIBRARY not found turning off OPENCV_FOUND")
	ENDIF (WIN32)
  ENDIF(OPENCV_HIGHGUI_LIBRARY)
ENDIF(LINK_LIB_HIGHGUI)

# get the link directory for rpath to be used with LINK_DIRECTORIES: 
IF (OPENCV_LIBRARY)
  GET_FILENAME_COMPONENT(OPENCV_LINK_DIRECTORIES ${OPENCV_LIBRARY} PATH)
ENDIF (OPENCV_LIBRARY)

# display help message
IF (NOT OPENCV_FOUND)
  MESSAGE("OPENCV library or headers not found. "
  "Please search manually or set env. variable OPENCV_ROOT to guide search." )
ENDIF (NOT OPENCV_FOUND)

MARK_AS_ADVANCED(
  OPENCV_INCLUDE_DIR
  OPENCV_INCLUDE_DIR_CXCORE
  OPENCV_INCLUDE_DIR_CV
  OPENCV_INCLUDE_DIR_CVAUX  
  OPENCV_INCLUDE_DIR_CVCAM
  OPENCV_INCLUDE_DIR_HIGHGUI  
  OPENCV_LIBRARIES
  OPENCV_LIBRARY
  OPENCV_HIGHGUI_LIBRARY
  OPENCV_CVAUX_LIBRARY
  OPENCV_CXCORE_LIBRARY
  OPENCV_CVCAM_LIBRARY
  OPENCV_DIR
)
