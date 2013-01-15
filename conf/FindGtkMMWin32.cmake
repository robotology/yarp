# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Alessandro Scalzo and Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Searches gtkmm on windows.
# Assumes that the environment variable GTKMM_BASEPATH is set to the place
# where GTKMM libs have been unpacked/installed. Users that want to install 
# gtkmm manually must define GTKMM_BASEPATH accordingly.
# 
# Added: 03/11/2011, Lorenzo
# Support gtkmm x64, search also using GTKMM64_BASEPATH (this variable is set by the installer).
#
# Creates:
# GtkMM_INCLUDE_DIRS   - Directories to include to use GTKMM
# GtkMM_LIBRARIES      - Files to link against to use GTKMM
# GtkMM_C_FLAGS        - Flags to pass to the C/C++ compiler for GTKMM.
# GtkMM_FOUND          - If false, don't try to use GTKMM
# GtkMM_VERSION        - version info
# GtkMM_MAJOR_VERSION  - major version
# GtkMM_MINOR_VERSION  - minor version

# 14/04/11, Lorenzo Natale: Added version info -- for now fake
# 03/11/11, Lorenzo Natale: Specifically handle Visual Studio 10 (see branch MSVC_VERSION EQUAL 1600) to avoid runtime error 
#           when running gyarpmanager. Previous version was linking vs9 lib/dlls. If VS 10 dlls/libs are not available with the
#           version of GTKMM you use, update to newer version (e.g. 2.22).



# prerequisite
FIND_PACKAGE(GtkPlus REQUIRED)
IF (NOT GtkPlus_FOUND)
    MESSAGE(SEND_ERROR "GtkPlus was not found but GtkMM requires GtkPlus.")
ENDIF (NOT GtkPlus_FOUND)

IF (GTKPLUS_C_FLAG)
	LIST(APPEND GTKMM_C_FLAGS ${GTKPLUS_C_FLAGS})
ENDIF (GTKPLUS_C_FLAG)

FIND_PACKAGE(PkgConfig)

IF(PKG_CONFIG_FOUND AND NOT WIN32)
    PKG_CHECK_MODULES(GtkMM gtkmm-2.4)
    IF (GtkMM_FOUND)
        MESSAGE(STATUS " pkg-config found gtkmm")
    ELSE (GtkMM_FOUND)
        MESSAGE(STATUS " pkg-config could not find gtkmm")
    ENDIF (GtkMM_FOUND)

else(PKG_CONFIG_FOUND AND NOT WIN32)
	# first check GTKMM64
	set(GTKMM_DIR  $ENV{GTKMM64_BASEPATH})
	# if that failed check GTKMM_BASEPATH
	if (NOT GTKMM_DIR )
		set(GTKMM_DIR  $ENV{GTKMM_BASEPATH})
	endif()
	
    # new vs. old style libraries detection (sort of fuzzy, temporary).
    # still uses a strict "find" which tests for a list of header files to 
    # be present and available. We might be able to relax this and just
    # glob everything under $GTKMM_DIR/lib $GTKMM/include
    #
    FILE(GLOB PATHGIOMM ${GTKMM_DIR}/lib/giomm*)
    IF (PATHGIOMM)
        SET (GTKMMVER "2.14.3")
    ELSE (PATHGIOMM)
        SET (GTKMMVER "2.8.x")
    ENDIF (PATHGIOMM)

    FILE(GLOB ALLSEARCHPATHS ${GTKMM_DIR}/include/*)
    FILE(GLOB TMPSP ${GTKMM_DIR}/lib/*)
    LIST(APPEND ALLSEARCHPATHS ${TMPSP})

    FOREACH (i ${ALLSEARCHPATHS})
        IF (IS_DIRECTORY ${i})
    #        MESSAGE("${i}") # testing!
        ELSE (IS_DIRECTORY ${i})
            LIST (REMOVE_ITEM ALLSEARCHPATHS ${i})
        ENDIF(IS_DIRECTORY ${i})
    ENDFOREACH (i)

    # complete the list
    LIST(APPEND ALLSEARCHPATHS ${GTKMM_DIR}/include)
    LIST(APPEND ALLSEARCHPATHS ${GTKMM_DIR}/lib)

    SET(HEADERTOSEARCH
        "libglademm"
        "libglademmconfig"
        "gtkmmconfig"
        "gtkmm"
        "gdkmmconfig"
        "gdkmm"
        "pangomm"
        "cairomm/cairomm"
        "atkmm"
        "libxml++config"
        "libxml++/libxml++"
        "glibmmconfig"
        "glibmm"
        "sigc++config"
        "sigc++/sigc++"

     )

    set(OPTIONAL_HEADERTOSEARCH
            "pangommconfig"
            "freetype/config/ftheader"
    )
                   
    # only new
    IF (GTKMMVER EQUAL "2.14.3")
        SET(HEADERTOSEARCH ${HEADERTOSEARCH} "giomm")
    ENDIF (GTKMMVER EQUAL "2.14.3")

    # old
    IF (GTKMMVER EQUAL "2.8.x")
        SET(HEADERTOSEARCH 
            ${HEADERTOSEARCH}
    #
        )
    ENDIF (GTKMMVER EQUAL "2.8.x")

    # optimistic :)
    SET(GtkMM_FOUND TRUE)
    SET(GTKMM_INCLUDE_DIRS ${GTKMM_DIR}/include)
    #MARK_AS_ADVANCED(FORCE GTKMM_TMP)

    FOREACH (i ${HEADERTOSEARCH})
        SET (GTKMM_TMP GTKMM_TMP-NOTFOUND CACHE INTERNAL "")
        FIND_PATH(GTKMM_TMP ${i}.h PATHS ${ALLSEARCHPATHS} PATH_SUFFIXES include)
        IF (GTKMM_TMP)
            LIST(APPEND GTKMM_INCLUDE_DIRS ${GTKMM_TMP})
        ELSE (GTKMM_TMP)
            SET(GtkMM_FOUND FALSE)
            MESSAGE("FindGtkMMWin32: ${i}.h not found, GtkMM is not available")
        ENDIF (GTKMM_TMP)
    ENDFOREACH (i)
    LIST(APPEND GTKMM_INCLUDE_DIRS ${GTKPLUS_INCLUDE_DIR})

    FOREACH (i ${OPTIONAL_HEADERTOSEARCH})
        SET (GTKMM_TMP GTKMM_TMP-NOTFOUND CACHE INTERNAL "")
        FIND_PATH(GTKMM_TMP ${i}.h PATHS ${ALLSEARCHPATHS} PATH_SUFFIXES include)
        IF (GTKMM_TMP)
            LIST(APPEND GTKMM_INCLUDE_DIRS ${GTKMM_TMP})
        ENDIF (GTKMM_TMP)
    ENDFOREACH (i)
    
    LIST(APPEND GTKMM_INCLUDE_DIRS ${GTKPLUS_INCLUDE_DIR})
    
    IF (MSVC_VERSION EQUAL 1400)
        SET(REGEX_GTKMM "[-](vc80[-])?")
    ELSE (MSVC_VERSION EQUAL 1400)
        IF (MSVC_VERSION EQUAL 1500)
            SET(REGEX_GTKMM "[-](vc90[-])?")
        ELSE (MSVC_VERSION EQUAL 1500)
		    IF (MSVC_VERSION EQUAL 1600)
			    SET(REGEX_GTKMM "[-](vc100[-])?")
	        ELSE(MSVC_VERSION EQUAL 1600)
				IF (MSVC_VERSION EQUAL 1700)
					MESSAGE("Warning, using VS10 libraray for Visual Studio 11")
					SET(REGEX_GTKMM "[-](vc100[-])?")
				ELSE (MSVC_VERSION EQUAL 1700)
					MESSAGE("Sorry, this version of FindGtkMMWin32.cmake does not yet support your version of Visual Studio")
					SET(GtkMM_FOUND FALSE)
				ENDIF (MSVC_VERSION EQUAL 1700)
            ENDIF (MSVC_VERSION EQUAL 1600)
		ENDIF (MSVC_VERSION EQUAL 1500)
    ENDIF (MSVC_VERSION EQUAL 1400)

    # I'd like to search for an unspecified version and pattern of library name (new format vs. old format)
    FILE(GLOB ALL_GTK_LIBS ${GTKMM_DIR}/lib/*.lib)

    #### MM specific libraries, all here except gthread, see later
    SET(LIBTOSEARCH 
        "xml++"
        "atkmm"
        "glademm"
        "gtkmm"
        "gdkmm"
        "pangomm"
        "glibmm"
        "sigc"
        "cairomm"
    )
    # new version
    IF (GTKMMVER EQUAL "2.14.3")
        SET(LIBTOSEARCH ${LIBTOSEARCH} "giomm")
    ENDIF (GTKMMVER EQUAL "2.14.3")

    FOREACH (i ${LIBTOSEARCH})
        SET (GTKMM_TMP_REL GTKMM_TMP-NOTFOUND CACHE INTERNAL "")
        SET (GTKMM_TMP_DBG GTKMM_TMP-NOTFOUND CACHE INTERNAL "")
        
        STRING(REPLACE "++" "[+][+]" j ${i})
        STRING(REGEX MATCHALL "${j}${REGEX_GTKMM}([0-9]([_]|[.])[0-9])+" LINK_LIBRARIES_WITH_PREFIX "${ALL_GTK_LIBS}")
        STRING(REGEX MATCHALL "(${j}${REGEX_GTKMM}([d][-])+([0-9]([_]|[.])[0-9])+)|(${j}([0-9]([_]|[.])[0-9])+([d])+)" LINK_LIBRARIES_WITH_PREFIX_DEBUG1 "${ALL_GTK_LIBS}")
        STRING(REGEX MATCHALL "${j}${REGEX_GTKMM}([0-9]([_]|[.])[0-9][d])+" LINK_LIBRARIES_WITH_PREFIX_DEBUG2 "${ALL_GTK_LIBS}")
        SET(LINK_LIBRARIES_WITH_PREFIX_DEBUG ${LINK_LIBRARIES_WITH_PREFIX_DEBUG1} ${LINK_LIBRARIES_WITH_PREFIX_DEBUG2})
        
        FIND_LIBRARY(GTKMM_TMP_REL NAMES ${LINK_LIBRARIES_WITH_PREFIX} PATHS ${GTKMM_DIR}/lib)
        IF (GTKMM_TMP_REL)
			LIST(APPEND GTKMM_LIBRARIES optimized ${GTKMM_TMP_REL})
        ELSE (GTKMM_TMP_REL)
            SET(GtkMM_FOUND FALSE)
            MESSAGE("Library ${i} optimized version not found, GtkMM doesn't seem to be available")
        ENDIF (GTKMM_TMP_REL)
    	
        FIND_LIBRARY(GTKMM_TMP_DBG NAMES ${LINK_LIBRARIES_WITH_PREFIX_DEBUG} PATHS ${GTKMM_DIR}/lib)
        IF (GTKMM_TMP_DBG)
            LIST(APPEND GTKMM_LIBRARIES debug ${GTKMM_TMP_DBG})
        ELSE (GTKMM_TMP_DBG)
            SET(GtkMM_FOUND FALSE)
            MESSAGE("Library ${i} debug version not found, GtkMM doesn't seem to be available")
        ENDIF (GTKMM_TMP_DBG)
    ENDFOREACH (i)

    ############### gthread does not have debug version
    ### I'll use gtk_tmp_rel variable for convenience
    SET (GTKMM_TMP_REL GTKMM_TMP-NOTFOUND CACHE INTERNAL "")
    FIND_LIBRARY(GTKMM_TMP_REL NAMES gthread-2.0 PATHS ${GTKMM_DIR}/lib)
    IF (GTKMM_TMP_REL)
		LIST(APPEND GTKMM_LIBRARIES optimized ${GTKMM_TMP_REL})
		LIST(APPEND GTKMM_LIBRARIES debug ${GTKMM_TMP_REL})
    ELSE (GTKMM_TMP_REL)
        SET(GtkMM_FOUND FALSE)
        MESSAGE("Library gthread not found, GtkMM doesn't seem to be available")
    ENDIF (GTKMM_TMP_REL)

    # complete list of link flags.
    LIST(APPEND GTKMM_LIBRARIES ${GTKPLUS_LINK_FLAGS})
    # MESSAGE("${GTKMM_LIBRARIES}")
    SET (GTKMM_TMP_DBG GTKMM_TMP_REL-NOTFOUND CACHE INTERNAL "")
    SET (GTKMM_TMP_REL GTKMM_TMP_DBG-NOTFOUND CACHE INTERNAL "")
    SET (GTKMM_TMP GTKMM_TMP-NOTFOUND CACHE INTERNAL "")

    set(GTKMM_C_FLAGS /wd4099 /wd4250 /wd4520)

    set(GtkMM_LIBRARIES ${GTKMM_LIBRARIES} CACHE STRING "Libraries for GtkMM")
    set(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS} CACHE STRING "Include directories for GtkMM")
    set(GtkMM_C_FLAGS ${GTKMM_C_FLAGS} CACHE STRING "C flags for GtkMM")

    ### For now we propagate this value. Much better version check 
    # can be done by checking gtkmmconfig.h, which defines 
    # GTKMM_MAJOR_VERSION and GTK_MINOR_VERSION
    set(GtkMM_VERSION ${GTKMMVER})
    
    ## split into major and minor
    string(REPLACE "." ";" GTKMM_VERSION_LIST ${GtkMM_VERSION})

    list(GET GTKMM_VERSION_LIST 0 GtkMM_VERSION_MAJOR)
    list(GET GTKMM_VERSION_LIST 1 GtkMM_VERSION_MINOR)

endif(PKG_CONFIG_FOUND AND NOT WIN32)

