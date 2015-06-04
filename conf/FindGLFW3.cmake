# Locate the glfw3 library
# This module defines the following variables:
# GLFW3_LIBRARY, the name of the library;
# GLFW3_INCLUDE_DIR, where to find glfw include files.
# GLFW3_FOUND, true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# GLFW3_ROOT which points to the root of the glfw3 library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw3 library:
#
# find_package(GLFW3 REQUIRED)
# add_executable(executable ${EXECUTABLE_SRCS})
# target_link_libraries (executable GLFW3::glfw3)
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.


include(MacroStandardFindModule)
macro_standard_find_module(GLFW3 glfw3)

if(NOT GLFW3_FOUND)

  find_path(GLFW3_INCLUDE_DIR
            DOC "Path to GLFW3 include directory."
            NAMES GLFW/glfw3.h
            PATH_SUFFIXES include
            PATHS /usr/
                  /usr/local/
                  ${GLFW3_ROOT_DIR}
                  ENV GLFW3_ROOT)

  find_library(GLFW3_GLFW_LIBRARY
               DOC "Absolute path to GLFW3 library."
               NAMES glfw3
               PATH_SUFFIXES lib
                             lib-vc2010
               PATHS /usr/
                     /usr/local/
                     ${GLFW3_ROOT_DIR}
                     ENV GLFW3_ROOT)

  find_library(OPENGL_LIBRARY
               NAMES OpenGL32
               PATHS "C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib")

  set(GLFW3_GLFW_glfw3_h "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h")
  if(GLFW3_INCLUDE_DIR AND EXISTS "${GLFW3_GLFW_glfw3_h}")
    file(STRINGS "${GLFW3_GLFW_glfw3_h}" GLFW3_GLFW_glfw3_h_CONTENTS
         REGEX "^#[\t ]*define[\t ]+GLFW3_VERSION_(MAJOR|MINOR|REVISION)[\t ]+[0-9]+$")

    foreach(_part MAJOR MINOR REVISION)
      string(REGEX REPLACE ".*#[\t ]*define[ \t]+GLFW3_VERSION_${_part}[ \t]+([0-9]+).*" "\\1"
             GLFW3_VERSION_${_part} "${GLFW3_GLFW_glfw3_h_CONTENTS}")
    endforeach(_part)

    set(GLFW3_VERSION_STRING "${GLFW3_VERSION_MAJOR}.${GLFW3_VERSION_MINOR}.${GLFW3_VERSION_REVISION}")

  endif()

  include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
  find_package_handle_standard_args(GLFW3
                                    REQUIRED_VARS GLFW3_GLFW_LIBRARY GLFW3_INCLUDE_DIR
                                    VERSION_VAR GLFW3_VERSION_STRING)
endif()


# Create imported target GLFW::glfw3
add_library(GLFW::glfw3 STATIC IMPORTED)
set_target_properties(GLFW::glfw3 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}"
  INTERFACE_LINK_LIBRARIES "${OPENGL_LIBRARY}")

# Import target "GLFW::glfw3" for configuration "Release"
set_property(TARGET GLFW::glfw3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GLFW::glfw3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${GLFW3_GLFW_LIBRARY}")
