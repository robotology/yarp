
# SOURCE: KDE
# File: [SVN] / trunk / KDE / kdesdk / cmake / modules / UsePkgConfig.cmake
# Revision: 501797, Mon Jan 23 22:03:18 2006 UTC by neundorf

# - pkg-config module for CMake
# Defines the following macros:
#   PKGCONFIG(package includedir libdir linkflags cflags)
#     - Calling PKGCONFIG will fill the desired information into the 4
#     given arguments, e.g. PKGCONFIG(libart-2.0 LIBART_INCLUDE_DIR
#     LIBART_LINK_DIR LIBART_LINK_FLAGS LIBART_CFLAGS) if pkg-config
#     was not found or the specified software package doesn't exist,
#     the variable will be empty when the function returns, otherwise
#     they will contain the respective information

FIND_PROGRAM(PKGCONFIG_EXECUTABLE NAMES pkg-config PATHS /usr/local/bin /usr/bin /sw/bin)
MARK_AS_ADVANCED(PKGCONFIG_EXECUTABLE)


IF(PKGCONFIG_EXECUTABLE)
	# test which interface is available
	EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --version RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _var)
	SET(MIN_PKGCONFIG_VERSION "0.19")
	IF(_var STRGREATER "${MIN_PKGCONFIG_VERSION}")
		SET(_MODERN_PKGCONFIG TRUE)
		MESSAGE(STATUS "Modern pkg-config utility detected")
	ELSE(_var STRGREATER "${MIN_PKGCONFIG_VERSION}")
		SET(_MODERN_PKGCONFIG FALSE)
		MESSAGE(STATUS "Older pkg-config utility detected")
	ENDIF(_var STRGREATER "${MIN_PKGCONFIG_VERSION}")
ENDIF(PKGCONFIG_EXECUTABLE)


MACRO(PKGCONFIG _package _include_DIR _link_DIR _link_FLAGS _cflags)
# reset the variables at the beginning
  SET(${_include_DIR})
  SET(${_link_DIR})
  SET(${_link_FLAGS})
  SET(${_cflags})

# if pkg-config has been found
  IF(PKGCONFIG_EXECUTABLE)

    EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --exists RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

# and if the package of interest also exists for pkg-config, then get the information
    IF(NOT _return_VALUE)

      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --variable=includedir OUTPUT_VARIABLE ${_include_DIR} )

      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --libs-only-L OUTPUT_VARIABLE ${_link_DIR} )

	IF(_MODERN_PKGCONFIG)
	      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --libs-only-l --libs-only-other OUTPUT_VARIABLE ${_link_FLAGS} )
	ELSE(_MODERN_PKGCONFIG)
	      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --libs-only-l OUTPUT_VARIABLE ${_link_FLAGS} )
	      # libs-only-L output will have surplus newline

	      # only cmake 2.2 compatible hack available right now is to delete when just a newline

		#	      STRING(LENGTH ${${_link_DIR}} _var)
		#	      IF (_var LESS 3)
		#		      SET(${_link_DIR} "")
		#	      ENDIF (_var LESS 3)

		#unfortunately, have to support cmake 2.0.5 -- no STRING LENGTH
		SET(${_link_DIR} "")
	ENDIF(_MODERN_PKGCONFIG)

	IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.1)
	      STRING(REGEX REPLACE "\n" " " ${_link_FLAGS} ${${_link_FLAGS}})
	      STRING(REGEX REPLACE "\n" " " ${_link_DIR} ${${_link_DIR}})
	      STRING(REGEX REPLACE "\n" " " ${_include_DIR} ${${_include_DIR}})
	ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.1)
	

      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --cflags OUTPUT_VARIABLE ${_cflags} )

    ENDIF(NOT _return_VALUE)

  ENDIF(PKGCONFIG_EXECUTABLE)

ENDMACRO(PKGCONFIG _include_DIR _link_DIR _link_FLAGS _cflags)

