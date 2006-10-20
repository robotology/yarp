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
#
# - nat modified to work on win: added "$ENV{OPENCV_DIR}/otherlibs/highgui/include"
# - nat, 19-oct 06: cxcore lib no longer mandatory on linux, but included only 
# if found. Still a requirement in windows (by nat)
# - nat, 20-oct 06: LINUX, we recommand you compile and install version opencv 0.9 
# from sourceforge. The Debian (stable) release are not compatible with the Windows
# version. If you installed the debian version of opencv make sure you remove them
# carefully as they might (will) clash...

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
  "$ENV{OPENCV_DIR}/otherlibs/highgui/"
  "$ENV{OPENCV_ROOT}"
  "$ENV{OPENCV_ROOT}/include"
  "$ENV{OPENCV_ROOT}/include/cv" 
  "$ENV{OPENCV_ROOT}/include/opencv" 
  "$ENV{OPENCV_ROOT}/cxcore/include"
  "$ENV{OPENCV_ROOT}/cv/include"
  "$ENV{OPENCV_ROOT}/cvaux/include"
  "$ENV{OPENCV_ROOT}/otherlibs/cvcam/include"
  "$ENV{OPENCV_ROOT}/otherlibs/highgui/include"
  "$ENV{OPENCV_ROOT}/otherlibs/highgui/"
  "$ENV{OPENCV_HOME}"
  "$ENV{OPENCV_HOME}/include"
  "$ENV{OPENCV_HOME}/include/cv"
  "$ENV{OPENCV_HOME}/include/opencv"
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
#  "$ENV{EXTRA}"
#  "$ENV{EXTRA}/include"
  /usr/include/opencv
  /usr/local/include/opencv
)
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
SET(OPENCV_POSSIBLE_LIBRARY_PATHS
  "$ENV{OPENCV_ROOT}/lib"
  "$ENV{OPENCV_ROOT}"
  "$ENV{OPENCV_DIR}"
  "$ENV{OPENCV_DIR}/lib"
  "$ENV{ProgramFiles}/OpenCV/lib"
#  "$ENV{EXTRA}"
#  "$ENV{EXTRA}/lib"
  "/usr/local/lib"
  "/usr/lib"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]/lib"
)

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
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS})

FIND_LIBRARY(OPENCV_CVAUX_LIBRARY
  NAMES cvaux cvaux0.9 cvaux0.9.5 cvaux0.9.6 cvaux0.9.7 cvaux0.9.8 cvaux0.9.9
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} )

FIND_LIBRARY(OPENCV_CXCORE_LIBRARY
  NAMES cxcore cxcore0.9  cxcore0.9.5 cxcore0.9.6 cxcore0.9.7 cxcore0.9.8 cxcore0.9.9
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} )

FIND_LIBRARY(OPENCV_HIGHGUI_LIBRARY
 NAMES highgui highgui0.9 highgui0.9.5 highgui0.9.6 highgui0.9.7 highgui0.9.8 highgui0.9.9
 PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} )
  
# optional CVCAM libs (WIN32 only)
FIND_LIBRARY(OPENCV_CVCAM_LIBRARY
  NAMES cvcam
  PATHS ${OPENCV_POSSIBLE_LIBRARY_PATHS} ) 

##
# Logic for required headers / include dirs
##

SET(OPENCV_FOUND ON)
FOREACH(INCDIR 
#  OPENCV_INCLUDE_DIR_CXCORE 
  OPENCV_INCLUDE_DIR_CV 
  OPENCV_INCLUDE_DIR_CVAUX 
#  OPENCV_INCLUDE_DIR_HIGHGUI 
  )
  IF    (${INCDIR})
    SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${${INCDIR}} )
    # MESSAGE("+ DBG ${INCDIR}=${${INCDIR}} ")
    # MESSAGE("+ DBG2 ${OPENCV_INCLUDE_DIR} ")
  ELSE  (${INCDIR})
    MESSAGE("- DBG ${INCDIR}=${${INCDIR}} ")
    SET(OPENCV_FOUND OFF)
  ENDIF (${INCDIR})  
ENDFOREACH(INCDIR)

# CVCAM exists only on Windows
IF   (OPENCV_INCLUDE_DIR_CVCAM)
  SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_CVCAM} )
ELSE (OPENCV_INCLUDE_DIR_CVCAM)
  # exists only on Windows, thus only there required
  IF    (WIN32)
    SET(OPENCV_FOUND OFF)
  ENDIF (WIN32)
ENDIF(OPENCV_INCLUDE_DIR_CVCAM)
# MESSAGE("DBG OPENCV_INCLUDE_DIR=${OPENCV_INCLUDE_DIR}")

IF (OPENCV_INCLUDE_DIR_CXCORE)
  SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_CXCORE})
ELSE (OPENCV_INCLUDE_DIR_CXCORE)
  IF (WIN32) #this is required on windows
	SET(OPENCV_FOUND OFF)
  ENDIF (WIN32)
ENDIF(OPENCV_INCLUDE_DIR_CXCORE)

#highgui does not seem to be required
IF (OPENCV_INCLUDE_DIR_HIGHGUI)
  SET(OPENCV_INCLUDE_DIR ${OPENCV_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR_HIGHGUI})
ELSE (OPENCV_INCLUDE_DIR_HIGHGUI)
  IF (WIN32)
	SET(OPENCV_FOUND OFF)
  ENDIF (WIN32)
ENDIF(OPENCV_INCLUDE_DIR_HIGHGUI)

##
# Logic for required libraries:
##
FOREACH(LIBNAME  
#  OPENCV_CXCORE_LIBRARY 
  OPENCV_LIBRARY 
  OPENCV_CVAUX_LIBRARY 
#  OPENCV_HIGHGUI_LIBRARY 
  )
  IF    (${LIBNAME})
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
  ENDIF (WIN32)
ENDIF(OPENCV_CXCORE_LIBRARY)

# CVCAM exists only on Windows
IF    (OPENCV_CVCAM_LIBRARY)
  SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${OPENCV_CVCAM_LIBRARY} )
ELSE  (OPENCV_CVCAM_LIBRARY)
  IF   (WIN32)
    SET(OPENCV_FOUND OFF)
  ENDIF (WIN32)
ENDIF (OPENCV_CVCAM_LIBRARY)
# MESSAGE("DBG OPENCV_LIBRARIES=${OPENCV_LIBRARIES}")

IF (OPENCV_HIGHGUI_LIBRARY)
  SET(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} ${OPENCV_HIGHGUI_LIBRARY})
ELSE (OPENCV_HIGHGUI_LIBRARY)
  IF (WIN32) #this is required on windows
	SET(OPENCV_FOUND OFF)
  ENDIF (WIN32)
ENDIF(OPENCV_HIGHGUI_LIBRARY)

# get the link directory for rpath to be used with LINK_DIRECTORIES: 
IF    (OPENCV_LIBRARY)
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
#  OPENCV_HIGHGUI_LIBRARY
  OPENCV_CVAUX_LIBRARY
  OPENCV_CXCORE_LIBRARY
  OPENCV_CVCAM_LIBRARY
)
