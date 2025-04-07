# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

create_device_test(opencvWriter)

yarp_install(
  FILES
    ./test.avi
  DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/tests/opencvWriter
)
