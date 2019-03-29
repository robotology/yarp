#.rst:
# FindPortAudio
# -------------
#
# Try to find the PortAudio library.

#=============================================================================
# Copyright 2019 Istituto Italiano di Tecnologia (IIT)
# Copyright 2008 RobotCub Consortium
#   Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#            Hatice Kose-Bagci
#            Daniele E. Domenichelli
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

# hacked together for unix by Paul Fitzpatrick
# updated to work on windows by Hatice Kose-Bagci


include(StandardFindModule)
include(FindPackageHandleStandardArgs)

standard_find_module(PortAudio portaudio-2.0)

if(NOT PortAudio_FOUND)
  if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(_suffix "x86")
    else()
      set(_suffix "x64")
    endif()
 
    find_library(PortAudio_LIBRARY_RELEASE
                 NAMES portaudio_${_suffix}
                       portaudio
                 PATHS "C:/portaudio/build/msvc/Debug_${_suffix}")
    mark_as_advanced(PortAudio_LIBRARY_RELEASE)

    find_library(PortAudio_LIBRARY_DEBUG
                 NAMES portaudio_${_suffix}
                       portaudio
                 PATHS "C:/portaudio/build/msvc/Debug_${_suffix}")
    mark_as_advanced(PortAudio_LIBRARY_DEBUG)

    include(SelectLibraryConfigurations)
    select_library_configurations(PortAudio)

    find_path(PortAudio_INCLUDE_DIR portaudio.h C:/portaudio/include)
    mark_as_advanced(PortAudio_INCLUDE_DIR)

    set(PortAudio_LIBRARIES ${PortAudio_LIBRARY})
    set(PortAudio_INCLUDE_DIRS ${PortAudio_INCLUDE_DIR})

    find_package_handle_standard_args(PortAudio DEFAULT_MSG PortAudio_LIBRARIES)
    set(PortAudio_FOUND ${PORTAUDIO_FOUND})

  endif()
endif()

# Compatibility
set(PORTAUDIO_LIBRARIES ${PortAudio_LIBRARIES})
set(PORTAUDIO_INCLUDE_DIR ${PortAudio_INCLUDE_DIR})
if(PortAudio_LIBRARY_RELEASE)
  get_filename_component(PORTAUDIO_LINK_DIRECTORIES "${PortAudio_LIBRARY_RELEASE}" DIRECTORY)
endif()

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(PortAudio PROPERTIES DESCRIPTION "Portable Cross-platform Audio I/O")
    set_package_properties(PortAudio PROPERTIES URL "http://www.portaudio.com/")
endif()
