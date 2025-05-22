# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

create_device_test(openCVGrabber)

yarp_install(
  FILES
    ./test.avi
  DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/tests/openCVGrabber
)
