/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SIMULATED_NETWORK_DELAY_H
#define SIMULATED_NETWORK_DELAY_H

#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>

 /**
  * @ingroup portmonitors_lists
  * \brief `simulated_network_delay`:  Documentation to be added
  */
class Simulated_network_delay : public yarp::os::MonitorObject
{
    double m_delay = 0.0;

public:
    void getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop);

    bool create(const yarp::os::Property &options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property &params) override;
    bool getparam(yarp::os::Property &params) override;

    void trig() override;

    bool accept(yarp::os::Things &thing) override;
    yarp::os::Things &update(yarp::os::Things &thing) override;
};

#endif
