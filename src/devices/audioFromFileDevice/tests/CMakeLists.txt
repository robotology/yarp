# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

create_device_test(audioFromFileDevice)

yarp_install(
  FILES
    ./440.wav
  DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/tests/audioFromFileDevice
)
