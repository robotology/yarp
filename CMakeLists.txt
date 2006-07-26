PROJECT(YARP)

# option offered while device library build behavior is untested
SET(CREATE_DEVICE_LIBRARY FALSE CACHE BOOL "Do you want to compile the device library")
# option for guis
SET(CREATE_GUIS FALSE CACHE BOOL "Do you want to compile GUIs")

# on windows, we have to tell ace how it was compiled
IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
ELSE (WIN32 AND NOT CYGWIN)
  # flush out warnings
  ADD_DEFINITIONS(-Wall)
ENDIF (WIN32 AND NOT CYGWIN)

## check if we are on cygwin
IF(WIN32 AND CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

#SET(USE_EXPERIMENTAL FALSE CACHE BOOL "Do you want to include experimental features?")
#IF (USE_EXPERIMENTAL)
#   ADD_DEFINITIONS(-DUSE_EXPERIMENTAL)
#ENDIF (USE_EXPERIMENTAL)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# this doesn't happen automatically for makefiles
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})
MAKE_DIRECTORY(${EXECUTABLE_OUTPUT_PATH})

MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# more msvc problems...
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/lib/Debug ${CMAKE_BINARY_DIR}/lib/Release)

# again, can't do this on windows without a lot of mess
#SET(BUILD_SHARED_LIBS ON)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)

# check endianness
IF(EXISTS "${CMAKE_ROOT}/Modules/TestBigEndian.cmake")
    INCLUDE(TestBigEndian)
    TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
    IF(${IS_BIG_ENDIAN})
        ADD_DEFINITIONS(-DYARP_BIG_ENDIAN)
    ELSE(${IS_BIG_ENDIAN})
        ADD_DEFINITIONS(-DYARP_LITTLE_ENDIAN)
    ENDIF(${IS_BIG_ENDIAN})
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/TestBigEndian.cmake")

# set the OS_TAG variable to "linux" or "winnt"

INCLUDE(UsePkgConfig)
INCLUDE(YarpTag)
YarpTag()

FIND_PACKAGE(Ace REQUIRED)

IF (CREATE_GUIS)
  FIND_PACKAGE(GtkPlus)
  IF (NOT GtkPlus_FOUND)
	MESSAGE(ERROR "gtk+ not found, won't compile dependent tools")
  ENDIF (NOT GtkPlus_FOUND)
ENDIF (CREATE_GUIS)

FIND_PACKAGE(Yarp1)

INCLUDE(YarpLibrary)
INCLUDE(YarpProgram)

INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ACE_INCLUDE_CONFIG_DIR})

ENABLE_TESTING()
SUBDIRS(src conf)

#
# Packing stuff
#
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Yet Another Robot Platform")
SET(CPACK_PACKAGE_NAME "yarp")
SET(CPACK_PACKAGE_VENDOR "YARP")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
INCLUDE(YarpVersion)
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")


#  if you are making debian packages, this is useful, but not otherwise
#CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/control.in
#               ${CMAKE_BINARY_DIR}/DEBIAN/control @ONLY IMMEDIATE)
#INSTALL_FILES(/doc/DEBIAN FILES ${CMAKE_BINARY_DIR}/DEBIAN/control)

IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

#  if you are trying to distribute, this is useful, but not otherwise
#  IF (WIN32 AND NOT CYGWIN)
#    INCLUDE(YarpInstallRequiredSystemLibraries)
#    INSTALL_FILES(/bin .dll ${ACE_LIBRARY} ${ACE_DEBUG_LIBRARY})
#    # just in case it is needed and not present
#    INSTALL_FILES(/bin FILES ${CMAKE_SOURCE_DIR}/conf/Microsoft.VC80.CRT.manifest)
#  ENDIF (WIN32 AND NOT CYGWIN)

  SET(CPACK_PACKAGE_EXECUTABLES "yarp" "yarp")
  INCLUDE(CPack)


ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
