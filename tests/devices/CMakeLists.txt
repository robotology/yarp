# Copyright: (C) 2009 RobotCub Consortium
# Copyright: (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


set(_inis test_motor_basic.ini)

if(ENABLE_yarpmod_test_grabber)
  list(APPEND _inis grabber_basic.ini
                    test_grabber_basic.ini)
endif()

if(ENABLE_yarpmod_ffmpeg_grabber)
  list(APPEND _inis ffmpeg_grabber_basic.ini
                    remote_grabber_basic.ini
                    ffmpeg_grabber_video4linux.ini)
  if(YARP_ENABLE_BROKEN_TESTS)
    list(APPEND _inis grabber_audiovisual.ini) # Fails with a segmentation fault
  endif()
endif()

if(ENABLE_yarpmod_ffmpeg_writer)
  list(APPEND _inis ffmpeg_writer_basic.ini)
endif()

if(ENABLE_yarpmod_opencv_grabber)
  list(APPEND _inis opencv_grabber_basic.ini)
endif()

if(ENABLE_yarpmod_portaudio)
  list(APPEND _inis grabber_audio.ini
                    portaudio_basic.ini)
endif()

if(NOT YARP_NO_DEPRECATED) # controlboard is deprecated since YARP 2.3.65
  list(APPEND _inis controlboard_basic.ini)
  if(ENABLE_yarpmod_test_grabber)
    list(APPEND _inis group_basic.ini)
  endif()
endif()

foreach(X ${_inis})
  get_filename_component(XN ${X} NAME_WE)
  get_filename_component(XINI ${X} ABSOLUTE)

  # need to cache examples for reference
  set(DOC_DIR "${CMAKE_SOURCE_DIR}/doc/device_invocation")
  set(XDOX ${DOC_DIR}/${XN}.dox)
  set(_cmd $<TARGET_FILE:harness_dev> --file "${XINI}" --doc "${XDOX}" --verbose)

  # Add normal test
  add_test(NAME "device::${XN}"
           COMMAND ${_cmd}
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  # Valgrind tests for devices are currently reporting issues
  if(YARP_ENABLE_BROKEN_TESTS)
    # Add valgrind tests
    foreach(_Tool ${YARP_VALGRIND_TOOLS})
      string(TOUPPER "${_Tool}" _TOOL)
      if(YARP_VALGRIND_${_TOOL}_TESTS)
        add_test(NAME "device::${XN}::Valgrind::${_Tool}"
                COMMAND ${VALGRIND_MEMCHECK_COMMAND} ${_cmd}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
      endif()
    endforeach()
  endif()

  # Add gdb tests
  if(YARP_GDB_TESTS)
    add_test(NAME "device::${XN}::gdb"
             COMMAND ${GDB_COMMAND} ${_cmd}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endforeach()
