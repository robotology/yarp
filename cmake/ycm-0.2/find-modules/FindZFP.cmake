#
# Find the ZFP includes and library
#

# This module defines
# ZFP_INCLUDE_DIR, where to find zfp.h
# ZFP_LIBRARIES, the libraries to link against
# ZFP_FOUND, if false, you cannot build anything that requires ZFP

######################################################################## 

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX "d") 
	find_library(ZFP_DEBUG_LIBRARY NAMES ZFP${CMAKE_DEBUG_POSTFIX} zfp${CMAKE_DEBUG_POSTFIX} PATHS $ENV{ZFP_ROOT}/lib $ENV{ZFP_ROOT} DOC "ZFP library file (debug version)")
        find_path(ZFP_INCLUDE_DIR NAMES zfp.h HINTS $ENV{ZFP_ROOT}/inc)
        find_library(ZFP_LIBRARY NAMES ZFP libzfp HINTS $ENV{ZFP_ROOT}/lib)
else(WIN32)
        find_path(ZFP_INCLUDE_DIR NAMES zfp.h HINTS $ENV{ZFP_ROOT}/inc)
        find_library(ZFP_LIBRARY NAMES ZFP libzfp.a HINTS $ENV{ZFP_ROOT}/lib)
endif(WIN32)
 
 
if (ZFP_INCLUDE_DIR AND ZFP_LIBRARY) 
	set(ZFP_FOUND TRUE) 
else () 
	set(ZFP_FOUND FALSE) 
endif () 
 
if (ZFP_DEBUG_LIBRARY) 
	set(ZFP_DEBUG_FOUND TRUE) 
else ()
  set(ZFP_DEBUG_LIBRARY ${ZFP_LIBRARY})
endif () 
 

# TSS: backwards compatibility
set(ZFP_LIBRARIES ${ZFP_LIBRARY}) 
