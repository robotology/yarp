#.rst:
# FindIPP
# -------
#
# Try to find the IPP library.

#=============================================================================
# Copyright 2010 RobotCub Consortium
#   Authors: Francesco Rea <francesco.rea@iit.it>
#            Giorgio Metta <giorgio.metta@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


SET(VERBOSE OFF)

IF(VERBOSE)
    MESSAGE(STATUS "Looking for IPP using FindIPP.cmake")
ENDIF(VERBOSE)

IF (NOT IPP_FOUND)
	SET(LINK_LIB_HIGHGUI TRUE CACHE BOOL "Do you want to link against libhighgui?")
	IF (LINK_LIB_HIGHGUI AND WIN32)
  		SET(LINK_LIB_CVCAM)
	ENDIF(LINK_LIB_HIGHGUI AND WIN32)

	SET(IS_GNUCXX3 FALSE)
	SET(IS_GNUCXX4 FALSE)
	IF    (${CMAKE_COMPILER_IS_GNUCXX})
		EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CXX_COMPILER_VERSION)
  
		IF   (CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")
			SET(IS_GNUCXX3 TRUE)
		ENDIF(CXX_COMPILER_VERSION MATCHES ".*3\\.[0-9].*")

		IF   (CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
			SET(IS_GNUCXX4 TRUE)
		ENDIF(CXX_COMPILER_VERSION MATCHES ".*4\\.[0-9].*")
	ENDIF (${CMAKE_COMPILER_IS_GNUCXX})

#----------- CHECK IPP_ROOT-----------------------------
IF (EXISTS "$ENV{IPP_ROOT}")
IF(VERBOSE)
MESSAGE("Exist IPP_ROOT. ")
ENDIF (VERBOSE)

SET(IPP_POSSIBLE_INCDIRS
	"$ENV{IPP_ROOT}"
	"$ENV{IPP_ROOT}/include"
)

SET(IPP_POSSIBLE_LIBRARY_PATHS
	"$ENV{IPP_ROOT}/lib"
	"$ENV{IPP_ROOT}/stublib"
	"$ENV{IPP_ROOT}/sharedlib"  
	"$ENV{IPP_ROOT}"
	"$ENV{IPP_ROOT}/sharedLib"
	"/opt/intel/ipp/sharedlib"
)
ENDIF (EXISTS "$ENV{IPP_ROOT}")
#--------------------------------------------------------

#-----------------CHECK IPP_DIR -----------------------
IF (EXISTS "$ENV{IPP_DIR}")
	IF(VERBOSE)
		MESSAGE("Exist IPP_DIR. ")
	ENDIF(VERBOSE)

	SET(IPP_POSSIBLE_INCDIRS
		"$ENV{IPP_DIR}"
		"$ENV{IPP_DIR}/include"
	)

	SET(IPP_POSSIBLE_LIBRARY_PATHS
		"$ENV{IPP_DIR}"
		"$ENV{IPP_DIR}/sharedlib"
		"$ENV{IPP_DIR}/lib"
		"$ENV{IPP_DIR}/stublib"
		"$ENV{IPP_DIR}/sharedLib" 
	)

ENDIF(EXISTS "$ENV{IPP_DIR}")
#----------------------------------------------------------

# CHECK IPP_HOME
IF (EXISTS "$ENV{IPP_HOME}")
	SET(IPP_POSSIBLE_INCDIRS
		"$ENV{IPP_HOME}"
		"$ENV{IPP_HOME}/include"
	)
ENDIF (EXISTS "$ENV{IPP_HOME}")

IF (NOT IPP_POSSIBLE_INCDIRS)
	IF(VERBOSE)
		MESSAGE('Not IPP_POSSIBLE INCDIRS')
	ENDIF(VERBOSE)

	SET(IPP_POSSIBLE_INCDIRS
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]"  
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]/include"
		"$ENV{ProgramFiles}/IPP"
		"$ENV{ProgramFiles}/IPP/include"
		/usr/include/IPP
		/opt/intel/ipp/include
		/opt/intel/ipp/.1.1.042/em64t/include	
		/usr/local/include/IPP)

ENDIF (NOT IPP_POSSIBLE_INCDIRS)

IF (NOT IPP_POSSIBLE_LIBRARY_PATHS)
	SET(IPP_POSSIBLE_LIBRARY_PATHS
		"$ENV{ProgramFiles}/IPP/lib"
		"/usr/local/lib"
		"/opt/intel/ipp/lib"
		"/opt/intel/ipp/sharedlib"
		"/usr/lib"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Intel(R) Open Source Computer Vision Library_is1;Inno Setup: App Path]/lib")
ENDIF(NOT IPP_POSSIBLE_LIBRARY_PATHS)

IF   (IS_GNUCXX3)
	SET(IPP_POSSIBLE_INCDIRS ${IPP_POSSIBLE_INCDIRS} 
		/opt/net/gcc33/IPP/
		/opt/net/gcc33/IPP/include
		/opt/net/gcc33/IPP/include/IPP )
ENDIF(IS_GNUCXX3)

IF   (IS_GNUCXX4)
	SET(IPP_POSSIBLE_INCDIRS ${IPP_POSSIBLE_INCDIRS} 
		/opt/net/gcc41/IPP/
		/opt/net/gcc41/IPP/include
		/opt/net/gcc41/IPP/include/IPP )
ENDIF(IS_GNUCXX4)

# candidates for IPP library directories:
IF   (IS_GNUCXX3)
	SET(IPP_POSSIBLE_LIBRARY_PATHS ${IPP_POSSIBLE_LIBRARY_PATHS}
		/opt/net/gcc33/IPP
		/opt/net/gcc33/IPP/lib )
ENDIF(IS_GNUCXX3)

IF   (IS_GNUCXX4)
	SET(IPP_POSSIBLE_LIBRARY_PATHS ${IPP_POSSIBLE_LIBRARY_PATHS}
		/opt/net/gcc41/IPP
		/opt/net/gcc41/IPP/lib
	)
ENDIF(IS_GNUCXX4)

IF(VERBOSE)
	MESSAGE("DBG (IPP_POSSIBLE_LIBRARY_PATHS=${IPP_POSSIBLE_LIBRARY_PATHS}")
ENDIF(VERBOSE)

# find (all) header files for include directories:
FIND_PATH(IPP_INCLUDE_DIR_CORE  ippcore.h ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_CORE)
	SET(IPP_INCLUDE_DIR_CORE "")
ENDIF(NOT IPP_INCLUDE_DIR_CORE)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_CORE)

FIND_PATH(IPP_INCLUDE_DIR_MAIN   ipp.h  ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_MAIN)
	SET(IPP_INCLUDE_DIR_MAIN "")
ENDIF(NOT IPP_INCLUDE_DIR_MAIN)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_MAIN)

FIND_PATH(IPP_INCLUDE_DIR_AC   ippac.h  ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_AC)
	SET(IPP_INCLUDE_DIR_AC "")
ENDIF(NOT IPP_INCLUDE_DIR_AC)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_AC)

FIND_PATH(IPP_INCLUDE_DIR_CC    ippcc.h      ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_CC)
	SET(IPP_INCLUDE_DIR_CC "")
ENDIF(NOT IPP_INCLUDE_DIR_CC)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_CC)

FIND_PATH(IPP_INCLUDE_DIR_CV    ippcv.h   ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_CV)
	SET(IPP_INCLUDE_DIR_CV "")
ENDIF(NOT IPP_INCLUDE_DIR_CV)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_CV)

FIND_PATH(IPP_INCLUDE_DIR_I    ippi.h   ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_I)
	SET(IPP_INCLUDE_DIR_I "")
ENDIF(NOT IPP_INCLUDE_DIR_I)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_I)

FIND_PATH(IPP_INCLUDE_DIR_S    ipps.h   ${IPP_POSSIBLE_INCDIRS} )
IF(NOT IPP_INCLUDE_DIR_S)
	SET(IPP_INCLUDE_DIR_S "")
ENDIF(NOT IPP_INCLUDE_DIR_S)
MARK_AS_ADVANCED(IPP_INCLUDE_DIR_S)

# find (all) libraries - some dont exist on Linux
FIND_LIBRARY(IPP_CC_LIBRARY
	NAMES ippcc 
	PATHS ${IPP_POSSIBLE_LIBRARY_PATHS}
	DOC "Location of the IPPcc")
MARK_AS_ADVANCED(IPP_CC_LIBRARY)

FIND_LIBRARY(IPP_S_LIBRARY
	NAMES ipps 
	PATHS ${IPP_POSSIBLE_LIBRARY_PATHS}
	DOC "Location of the IPPs")
MARK_AS_ADVANCED(IPP_S_LIBRARY)

FIND_LIBRARY(IPP_CV_LIBRARY
	NAMES ippcv 
	PATHS ${IPP_POSSIBLE_LIBRARY_PATHS} 
	DOC "Location of the cvaux IPPcv")
MARK_AS_ADVANCED(IPP_CV_LIBRARY)

FIND_LIBRARY(IPP_I_LIBRARY
	NAMES ippi
	PATHS ${IPP_POSSIBLE_LIBRARY_PATHS} 
	DOC "Location of the IPPi")
MARK_AS_ADVANCED(IPP_I_LIBRARY)
  
FIND_LIBRARY(IPP_CORE_LIBRARY
	NAMES ippcore
	PATHS ${IPP_POSSIBLE_LIBRARY_PATHS} 
	DOC "Location of the IPPcore")
MARK_AS_ADVANCED(IPP_CORE_LIBRARY)

########
SET(IPP_FOUND ON)
########

###########################################
## CHECK HEADER FILES
# REQUIRED LIBS
FOREACH(INCDIR 
	IPP_INCLUDE_DIR_CV 
	IPP_INCLUDE_DIR_CC	
	IPP_INCLUDE_DIR_I
	IPP_INCLUDE_DIR_S
	IPP_INCLUDE_DIR_MAIN
	IPP_INCLUDE_DIR_CORE
	)

	IF (${INCDIR})
		SET(IPP_INCLUDE_DIRS ${IPP_INCLUDE_DIRS} ${${INCDIR}} )
		IF(VERBOSE)
			MESSAGE("${IPP_INCLUDE_DIRS}  found for ${INCDIR}.keeping on IPP_FOUND ")
		ENDIF (VERBOSE)
	ELSE (${INCDIR})
		IF(VERBOSE)
			MESSAGE("${IPP_INCLUDE_DIRS} not found turning off IPP_FOUND")
		ENDIF(VERBOSE)
		SET(IPP_FOUND OFF)
		SET(IPP_INCLUDE_DIR_MAIN "")
	ENDIF (${INCDIR})  

ENDFOREACH(INCDIR)

#################################

## LIBRARIES
SET (IPP_LIBRARIES_PATH ${IPP_LIBRARIES_PATH} ${IPP_POSSIBLE_LIBRARY_PATHS})
IF(VERBOSE)
	MESSAGE("IPP_LIBRARIES_PATH set as ${IPP_LIBRARIES_PATH} .keeping on IPP_FOUND")
ENDIF(VERBOSE)

# REQUIRED LIBRARIES
FOREACH(LIBNAME  
	IPP_I_LIBRARY 
	IPP_S_LIBRARY
	IPP_CC_LIBRARY
	IPP_CORE_LIBRARY
	IPP_CV_LIBRARY)

	IF (${LIBNAME})
		SET(IPP_LIBRARIES ${IPP_LIBRARIES} ${${LIBNAME}} )
		IF(VERBOSE)
			MESSAGE("${LIBNAME} found for ${LIBNAME} .keeping on IPP_FOUND")
		ENDIF(VERBOSE)
	ELSE  (${LIBNAME})
		IF(VERBOSE)
			MESSAGE("${LIBNAME} not found turning off IPP_FOUND")
		ENDIF(VERBOSE)
		SET(IPP_FOUND OFF)
	ENDIF (${LIBNAME})

ENDFOREACH(LIBNAME)

# get the link directory for rpath to be used with LINK_DIRECTORIES: 
IF (IPP_LIBRARY)
	GET_FILENAME_COMPONENT(IPP_LINK_DIRECTORIES ${IPP_LIBRARY} PATH)
ENDIF (IPP_LIBRARY)

# display help message
IF (NOT IPP_FOUND)
	IF(VERBOSE)
		MESSAGE("IPP library or headers not found. "
		"Please search manually or set env. variable IPP_ROOT to guide search." )
	ENDIF(VERBOSE)
ENDIF (NOT IPP_FOUND)

MARK_AS_ADVANCED(
	IPP_INCLUDE_DIRS  
	IPP_LIBRARIES
	IPP_LIBRARY
	IPP_DIR
)

ENDIF (NOT IPP_FOUND)
