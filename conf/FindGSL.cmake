# 
# Try to find GSL library
# Once run this will define: 
# 
# GSL_FOUND
# GSL_INCLUDE_DIR
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


# CHECK GSL_ROOT
IF (EXISTS "$ENV{GSL_ROOT}")
  SET(GSL_INCLUDE_DIR "$ENV{GSL_ROOT}/include")
  SET(GSL_LINK_DIRECTORIES "$ENV{GSL_ROOT}/lib")

  FIND_LIBRARY(GSL_LIBRARIES
	NAMES libgsl libgsl.lib
	PATHS ${GSL_LINK_DIRECTORIES} 
	DOC "Location of the gsl lib") 

  FIND_LIBRARY(GSLCBLAS_LIBRARIES
	NAMES libgslcblas libgslcblas.lib
	PATHS ${GSL_LINK_DIRECTORIES} 
	DOC "Location of the gslcblas lib")

  SET(GSL_FOUND ON)

ENDIF (EXISTS "$ENV{GSL_ROOT}")

# display help message
IF (NOT GSL_FOUND)
  MESSAGE("GSL library or headers not found. "
  "Please search manually or set env. variable GSL_ROOT to guide search." )
ENDIF (NOT GSL_FOUND)

#MARK_AS_ADVANCED(
#  GSL_INCLUDE_DIR
#  GSL_LINK_DIRECTORIES
#  GSL_LIBRARIES
#  GSLCBLAS_LIBRARIES
#)
