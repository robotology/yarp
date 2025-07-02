/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PM_THROTTLEDOWN
#define YARP_PM_THROTTLEDOWN

#include <yarp/os/Property.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>

#include <string>
#include <vector>

 /**
  * @ingroup portmonitors_lists
  * \brief `ThrottleDown`:
  * Portmonitor plugin for reducing the throughput of a port connection, decimating the transmitted messages.
  * For example, this portmonitor can be used to reduce a port streamed data from 10 to 1 message per second,
  * reducing the overall bandwidth usage. Please note that this portmonitor can be attached to both the
  * sender and the receiver port, however bandwidth consumption will be reduced only if the portmonitor is
  * attached to the sender port.
  *
  * Example usage:
  * yarp write /test --period 0.01
  * yarp read ... /in
  * yarp connect  /test /in tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.500
  */
class ThrottleDown : public yarp::os::MonitorObject
{
    double m_last_time = 0;
    double m_period = 1.0;

public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

    void getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop);
};

#endif  // YARP_PM_THROTTLEDOWN
