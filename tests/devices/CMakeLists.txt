# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_devices)
target_sources(harness_devices PRIVATE harness_devices.cpp)
target_link_libraries(harness_devices PRIVATE YARP::YARP_os
                                              YARP::YARP_init
                                              YARP::YARP_dev)
set_property(TARGET harness_devices PROPERTY FOLDER "Test")

set(_inis AudioPlayerWrapper_fakeSpeaker.ini
          AudioPlayerWrapper_portaudio.ini
          AudioPlayerWrapper_portaudioPlayer.ini
          AudioRecorderWrapper_fakeMicrophone.ini
          AudioRecorderWrapper_portaudio.ini
          AudioRecorderWrapper_portaudioRecorder.ini
          controlboardwrapper2_basic.ini
          ffmpeg_grabber_basic.ini
          ffmpeg_grabber_video4linux.ini
          ffmpeg_writer_basic.ini
          grabber_basic.ini
          grabber_audio.ini
          group_basic.ini
          opencv_grabber_basic.ini
          portaudio_basic.ini
          remote_grabber_basic.ini
          fakeFrameGrabber_basic.ini
          fakeMotionControl_basic.ini
          usbCamera.ini)

if(YARP_ENABLE_BROKEN_TESTS)
  # Randomly fails with a segmentation fault
  list(APPEND _inis grabber_audiovisual.ini)
endif()

foreach(_ini_file ${_inis})
  get_filename_component(_test ${_ini_file} NAME_WE)
  get_filename_component(_ini_file ${_ini_file} ABSOLUTE)

  # need to cache examples for reference
  set(_out_dox "${CMAKE_SOURCE_DIR}/doc/device_invocation/${_test}.dox")
  set(_cmd ${YARP_TEST_LAUNCHER} $<TARGET_FILE:harness_devices> --file "${_ini_file}" --doc "${_out_dox}" --verbose)

  # Add normal test
  add_test(NAME "device::${_test}"
           COMMAND ${_cmd}
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(TEST "device::${_test}" PROPERTY SKIP_RETURN_CODE 254)
  set_property(TEST "device::${_test}" PROPERTY TIMEOUT ${YARP_TEST_TIMEOUT})
endforeach()
