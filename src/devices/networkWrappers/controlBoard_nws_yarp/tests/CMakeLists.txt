# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# The RPC-interface section of the test talks to the device through the
# thrift-generated client, so the test also needs the generated messages.
create_device_test(controlBoard_nws_yarp ControlBoardMsgs)
