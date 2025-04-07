/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yDeviceDescription {
} (
  yarp.name = "yarp::dev::DeviceDescription"
  yarp.includefile = "yarp/dev/IRobotDescription.h"
)

struct return_getAllDevices {
  1: yReturnValue ret;
  2: list<yDeviceDescription> devices;
}

struct return_getAllDevicesByType {
  1: yReturnValue ret;
  2: list<yDeviceDescription> devices;
}


service IRobotDescriptionMsgs
{
  return_getAllDevices               getAllDevicesRPC        ();
  return_getAllDevicesByType         getAllDevicesByTypeRPC  (1: string type);
  yReturnValue                       registerDeviceRPC       (1: yDeviceDescription dev);
  yReturnValue                       unregisterDeviceRPC     (1: string dev);
  yReturnValue                       unregisterAllRPC        ();
}
