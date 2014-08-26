# Copyright:(C) 2008 RobotCub Consortium
# Authors: Paul Fitzpatrick, Hatice Kose-Bagci
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# hacked together for unix by Paul Fitzpatrick
# updated to work on windows by Hatice Kose-Bagci


include(MacroStandardFindModule)
include(FindPackageHandleStandardArgs)

macro_standard_find_module(PortAudio portaudio-2.0)

if(NOT PortAudio_FOUND)
  if(WIN32)
    find_library(PortAudio_LIBRARY NAMES portaudio_x86 PATHS C:/portaudio/build/msvc/Debug_x86)
    get_filename_component(PORTAUDIO_LINK_DIRECTORIES ${PortAudio_LIBRARY} PATH)
    find_path(PortAudio_INCLUDE_DIR portaudio.h C:/portaudio/include)
    set(PortAudio_LIBRARIES ${PortAudio_LIBRARY})

    #message(STATUS "inc: -${PortAudio_INCLUDE_DIR}")
    #message(STATUS "link flags: -${PortAudio_LINK_FLAGS}")
    #message(STATUS "cflags: -${PortAudio_CFLAGS}")
    #message(STATUS "link libs: -${PortAudio_LIBRARIES}")

    find_package_handle_standard_args(PortAudio DEFAULT_MSG PortAudio_LIBRARIES)
    set(PortAudio_FOUND ${PORTAUDIO_FOUND})

  endif()
endif()

# Compatibility
set(PORTAUDIO_LIBRARIES ${PortAudio_LIBRARIES})
set(PORTAUDIO_INCLUDE_DIR ${PortAudio_INCLUDE_DIR})
# Set package properties if FeatureSummary was included

if(COMMAND set_package_properties)
    set_package_properties(PortAudio PROPERTIES DESCRIPTION "Portable Cross-platform Audio I/O")
    set_package_properties(PortAudio PROPERTIES URL "http://www.portaudio.com/")
endif()

