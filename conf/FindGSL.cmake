# Copyright: (C) 2010 Jan Woetzel
# Small modifications by Lorenzo Natale and Francesco Nori
# CopyPolicy: Use under LGPL or BSD license at your choice

# 
# Try to find GSL library
# Once run this will define: 
# 
# GSL_FOUND
# GSL_INCLUDE_DIR and GSL_INCLUDE_DIRS
# GSL_LIBRARIES
# GSL_LINK_DIRECTORIES
##

# 
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006 complete rewrite by Jan Woetzel
##
# www.mip.informatik.uni-kiel.de/
# --------------------------------
#
# Modified by macl
#


# Included in yarp by nat March 07.
# Some changes to account for different include dirs.
# TODO: check if it works on linux.

##### check GSL_ROOT
IF (EXISTS "$ENV{GSL_ROOT}")
  SET(GSL_POSSIBLE_INCDIRS
	"$ENV{GSL_ROOT}/include"
	"$ENV{GSL_ROOT}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS
	"$ENV{GSL_ROOT}/lib")
ENDIF (EXISTS "$ENV{GSL_ROOT}")

##### check GSL_DIR
IF (EXISTS "$ENV{GSL_DIR}")
  SET(GSL_POSSIBLE_INCDIRS
	"$ENV{GSL_DIR}/include"
	"$ENV{GSL_DIR}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS
	"$ENV{GSL_DIR}/lib")
ENDIF (EXISTS "$ENV{GSL_DIR}")

IF (GSL_DIR)
  SET(GSL_POSSIBLE_INCDIRS
	"${GSL_DIR}/include"
	"${GSL_DIR}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS
	"${GSL_DIR}/lib")
ENDIF (GSL_DIR)

FIND_PATH(GSL_BLAS_HEADER gsl/gsl_blas.h  
	${GSL_POSSIBLE_INCDIRS} 
	/usr/include
	/usr/local/include
	)

FIND_LIBRARY(GSL_LIBRARY
  NAMES libgsl libgsl.lib gsl
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib")

FIND_LIBRARY(GSLCBLAS_LIBRARY
  NAMES libgslcblas libgslcblas.lib gslcblas
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib")

IF (GSLCBLAS_LIBRARY AND GSL_LIBRARY AND GSL_BLAS_HEADER)
  SET(GSL_INCLUDE_DIR "${GSL_BLAS_HEADER}")
  SET(GSL_INCLUDE_DIRS "${GSL_BLAS_HEADER}")
  SET(GSL_LINK_DIRECTORIES "$ENV{GSL_ROOT}/lib")
  SET(GSL_LIBRARIES "${GSL_LIBRARY}")
  SET(GSL_LIBRARIES "${GSL_LIBRARIES}" "${GSLCBLAS_LIBRARY}")
  SET(GSL_FOUND ON)
  SET(GSL_DIR "$ENV{GSL_ROOT}")
ENDIF (GSLCBLAS_LIBRARY AND GSL_LIBRARY AND GSL_BLAS_HEADER)

SET (GSL_MARK)

IF (GSL_FOUND)
  # nothing to say
  IF (NOT GSL_DIR)
	MARK_AS_ADVANCED(GSL_DIR)
  ENDIF (NOT GSL_DIR)
ELSE (GSL_FOUND)
  SET (GSL_DIR "" CACHE PATH "Location of GSL")
  IF (GSL_FIND_REQUIRED OR GSL_DIR)
    SET (GSL_MARK CLEAR)
    MARK_AS_ADVANCED(
      ${GSL_MARK}
      GSL_INCLUDE_DIR
      GSL_LINK_DIRECTORIES
      GSL_LIBRARIES
      GSLCBLAS_LIBRARIES
      GSLCBLAS_LIBRARY
      GSL_BLAS_HEADER
      GSL_LIBRARY
    )
    MESSAGE(FATAL_ERROR "GSL library or headers not found. "
            "Please search manually or set env. variable GSL_DIR to guide search." )
  ENDIF (GSL_FIND_REQUIRED OR GSL_DIR)  
ENDIF (GSL_FOUND)


MARK_AS_ADVANCED(
  ${GSL_MARK}
  GSL_INCLUDE_DIR
  GSL_INCLUDE_DIRS
  GSL_LINK_DIRECTORIES
  GSL_LIBRARIES
  GSLCBLAS_LIBRARIES
  GSLCBLAS_LIBRARY
  GSL_BLAS_HEADER
  GSL_LIBRARY
)

