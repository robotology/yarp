######################
#
#    Copyright (C) 2014 EngSaS - Engineering Solutions and Services Langenbach. All rights reserved.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
######################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(qgv)

SET(TARGET_VERSION_MAJOR 0 )
    SET(TARGET_VERSION_MINOR 1 )
    SET(${PROJECT_NAME}_PATCH_LEVEL   0 )
SET(${PROJECT_NAME}_SOVERSION ${TARGET_VERSION_MAJOR})
SET(${PROJECT_NAME}_VERSION
    "${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}" )

# INCLUDE(CTest)

# Extending the Module path with the local module path of desuto
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

OPTION(BUILD_DEV "Build development version of qgv" ON)
OPTION(BUILD_DEBUG "Build in debug mode" ON)
OPTION(BINDINGS_QT4 "Build with Qt4" OFF)
OPTION(BINDINGS_QT5 "Build with Qt5" OFF)

SET(BUILD_DEBUG_WANTED ${BUILD_DEBUG})
SET(BUILD_DEV_WANTED ${BUILD_DEV})
SET(BINDINGS_QT4_WANTED ${BINDINGS_QT4})
SET(BINDINGS_QT5_WANTED ${BINDINGS_QT5})

IF(BUILD_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
ENDIF()

IF(NOT BINDINGS_QT4 AND NOT BINDINGS_QT5)
    MESSAGE("Trying to autodetect Qt version")
    FIND_PACKAGE(Qt4 QUIET)
    IF(Qt4_FOUND OR QT4_FOUND)
        SET(BINDINGS_QT4 ON)
        MESSAGE("Found Qt4 --> Prefer Qt4 over Qt5")
    ELSE(Qt4_FOUND OR QT4_FOUND)
        SET(BINDINGS_QT5 ON)
        MESSAGE("Qt4 not found --> Try to build with Qt5")
    ENDIF(Qt4_FOUND OR QT4_FOUND)
ENDIF(NOT BINDINGS_QT4 AND NOT BINDINGS_QT5)
IF(BINDINGS_QT4)
        # Qt4 must be included after setting the build type
        FIND_PACKAGE(Qt4 REQUIRED)
ENDIF(BINDINGS_QT4)
IF(BINDINGS_QT5)
        IF(BINDINGS_QT4)
                MESSAGE("Qt4 and Qt5 enabled --> Preferring Qt4 and therefore disabling Qt5")
                SET(BINDINGS_QT5  OFF)
        ELSE(BINDINGS_QT4)
                FIND_PACKAGE(Qt5Core REQUIRED)
                FIND_PACKAGE(Qt5Widgets REQUIRED)
                FIND_PACKAGE(Qt5Xml REQUIRED)
                FIND_PACKAGE(Qt5XmlPatterns REQUIRED)
                FIND_PACKAGE(Qt5Sql REQUIRED)
                FIND_PACKAGE(Qt5Network REQUIRED)
                FIND_PACKAGE(Qt5LinguistTools REQUIRED)
                # Do not automatically link against qtmain
                CMAKE_POLICY(SET CMP0020 OLD)
        ENDIF(BINDINGS_QT4)
ENDIF(BINDINGS_QT5)
# enable automatic moc by CMAKE
SET(CMAKE_AUTOMOC ON)

IF(NOT KDE)
	IF(NOT APPLE)
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
	ENDIF(NOT APPLE)
	IF(MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_EXCEPTIONS -Zc:wchar_t-")
	ELSE(MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-check-new -fno-common")
	ENDIF(MSVC)
	IF(NOT WIN32 AND NOT APPLE)
#		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--fatal-warnings -lc -Wl,--disable-auto-import")
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--fatal-warnings -lc")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")
	ENDIF(NOT WIN32 AND NOT APPLE)
ENDIF(NOT KDE)
IF(BUILD_DEBUG AND NOT MSVC)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
ENDIF(BUILD_DEBUG AND NOT MSVC)
ADD_DEFINITIONS(${QT_DEFINITIONS})

# directory to install binaries
SET(bin_DIR ${CMAKE_INSTALL_PREFIX}/bin)
IF(WIN32)
	SET(bin_DIR .)
ENDIF(WIN32)

IF(NOT WIN32)
	SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/" CACHE STRING "Where to install the library files to")
ENDIF(NOT WIN32)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
IF(UNIX)
#  SET(designer_DIR "/usr/lib/qt4/plugins/designer")
	SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	SET(include_DIR ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
	SET(find_cmake_DIR ${CMAKE_ROOT}/Modules/)
ELSE(UNIX)
	SET(LIB_INSTALL_DIR ${bin_DIR})
	SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
	SET(include_DIR ${bin_DIR}/include/${PROJECT_NAME})
	SET(find_cmake_DIR ${CMAKE_ROOT}/Modules/)
ENDIF(UNIX)

IF(WIN32)
  IF(BUILD_DEBUG)
    SET(qt_LIBS ${qt_LIBS}/Debug)
  ELSE(BUILD_DEBUG)
    SET(qt_LIBS ${qt_LIBS}/Release)
  ENDIF(BUILD_DEBUG)
ENDIF()

FIND_PACKAGE(Qt4 REQUIRED)
ADD_DEFINITIONS(${QT_DEFINITIONS})
FIND_PACKAGE(Graphviz REQUIRED)

######################
#
# include source tree
#
######################
SET(libqgv_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/QGVCore)
SET(libqgv_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/QGVCore)
ADD_SUBDIRECTORY(QGVCore)
ADD_SUBDIRECTORY(Sample)

######################
#
# printing choosen configuration
#
######################
MESSAGE("######################################################")
MESSAGE("")
MESSAGE("CONFIGURATION")
MESSAGE("")
MESSAGE("CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
MESSAGE("CMAKE_CXX_FLAGS:           ${CMAKE_CXX_FLAGS}")
MESSAGE("CMAKE_INSTALL_PREFIX:      ${CMAKE_INSTALL_PREFIX}")
MESSAGE("")
MESSAGE("OPTIONS:")
MESSAGE("  Name                               Wanted     Used")
# MESSAGE("  BUILD_PACKAGE:                     ${BUILD_PACKAGE_WANTED}	${BUILD_PACKAGE}")
MESSAGE("  BUILD_DEBUG:                       ${BUILD_DEBUG_WANTED}		${BUILD_DEBUG}")
# MESSAGE("  BUILD_TESTING:                     ${BUILD_TESTING_WANTED}		${BUILD_TESTING}")
MESSAGE("  BUILD_DEV:                         ${BUILD_DEV}	${BUILD_DEV}")
MESSAGE("  BINDINGS_QT4:                      ${BINDINGS_QT4_WANTED}		${BINDINGS_QT4}")
MESSAGE("  BINDINGS_QT5:                      ${BINDINGS_QT5_WANTED}		${BINDINGS_QT5}")
# MESSAGE("  L10N_ALL:                          ${L10N_ALL_WANTED}		${L10N_ALL}")
# MESSAGE("    L10N_DE:                         ${L10N_DE_WANTED}		${L10N_DE}")
# MESSAGE("    L10N_EN:                         ${L10N_EN_WANTED}		${L10N_EN}")
# MESSAGE("    L10N_FR:                         ${L10N_FR_WANTED}		${L10N_FR}")
MESSAGE("")
MESSAGE("######################################################")

######################
#
# Add uninstall target
#
######################
CONFIGURE_FILE(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")


######################
#
# CPACK
#
######################
SET(CPACK_GENERATOR "NSIS")
SET(CPACK_PACKAGE_NAME "QGV")
SET(CPACK_PACKAGE_VENDOR "")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt interface for Graphviz")
SET(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${TARGET_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${TARGET_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_PATCH_LEVEL})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "qgv")

# add all components which should be installable via the package
IF(WIN32)
  SET(external_DLL
    qtlibs
    graphvizlibs
  )
ENDIF(WIN32)

SET(CPACK_COMPONENTS_ALL
  library
  ${external_DLL}
  headerfiles
	sample
)

# License file
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)

# Pre selected installation profiles
SET(CPACK_ALL_INSTALL_TYPES Full Minimal)

# groups for components
SET(CPACK_COMPONENT_GROUP_3RDPARTYLIBS_DISPLAY_NAME "3rd Party Libraries")
SET(CPACK_COMPONENT_GROUP_3RDPARTYLIBS_DESCRIPTION "Libraries which are needed, but not belonging to this project. They must not be installed, if they are already installed at path")

# the scheme of the variable names are: CPACK_COMPNONENT_%COMPONENTNAMEINUPPERCASE%_DISPLAY_NAME
# where %COMPONENTNAMEINUPPERCASE% must be replaced by the upper cased name of the component
# see below for example
SET(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "QGV")
SET(CPACK_COMPONENT_LIBRARY_DESCRIPTION
   "The qgv library")
# SET(CPACK_COMPONENT_LIBRARY_GROUP "Base")
SET(CPACK_COMPONENT_LIBRARY_INSTALL_TYPES Full Minimal)

IF(BUILD_DEV)
  SET(CPACK_COMPONENT_HEADERFILES_DISPLAY_NAME "Header files")
  SET(CPACK_COMPONENT_HEADERFILES_DESCRIPTION
    "Header files of qgv library")
  SET(CPACK_COMPONENT_HEADERFILES_DEPENDS library)
  SET(CPACK_COMPONENT_HEADERFILES_INSTALL_TYPES Full)
ENDIF(BUILD_DEV)

SET(CPACK_COMPONENT_QTLIBS_DISPLAY_NAME "QT")
SET(CPACK_COMPONENT_QTLIBS_DESCRIPTION
   "Qt libraries")
SET(CPACK_COMPONENT_QTLIBS_GROUP "3rdPartyLibs")
SET(CPACK_COMPONENT_QTLIBS_INSTALL_TYPES Full)

SET(CPACK_COMPONENT_GRAPHVIZLIBS_DISPLAY_NAME "Graphviz")
SET(CPACK_COMPONENT_GRAPHVIZLIBS_DESCRIPTION
   "Graphviz libraries")
SET(CPACK_COMPONENT_GRAPHVIZLIBS_GROUP "3rdPartyLibs")
SET(CPACK_COMPONENT_GRAPHVIZLIBS_INSTALL_TYPES Full)

SET(CPACK_COMPONENT_SAMPLE_DISPLAY_NAME "Sample")
SET(CPACK_COMPONENT_SAMPLE_DESCRIPTION
   "Example application")
SET(CPACK_COMPONENT_SAMPLE_DEPENDS library)
SET(CPACK_COMPONENT_SAMPLE_INSTALL_TYPES Full)

# used by NSIS to create startmenu links to the executables
# this entrys are only useful if a bin folder is used, otherwise use the empty EXECUTABLES
# and fill out the NSIS_MENU_LINKS
SET(CPACK_PACKAGE_EXECUTABLES ${CPACK_PACKAGE_EXECUTABLES} "Sample" "Sample")

# This must always be last!
INCLUDE(CPack)
