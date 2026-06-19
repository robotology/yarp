# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

create_device_test(fakeCalibrator YARP::YARP_robotinterface)

yarp_install(
  FILES
    ./RobotinterfaceWithFakeCalibrator.xml
  DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/tests/libYARP_robotinterface
)
