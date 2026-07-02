/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TEST_NUMBERS_H
#define TEST_NUMBERS_H

#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

 /**
  * @ingroup portmonitors_lists
  * \brief `test_numbers`:  Documentation to be added
  */
class Test_numbers1 : public yarp::os::MonitorObject
{
    yarp::os::Things m_th;
    int m_valueoperation = 0;

public:
    void getParamsFromCommandLine(std::string carrierString, yarp::os::Property& prop);

    bool create(const yarp::os::Property &options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property &params) override;
    bool getparam(yarp::os::Property &params) override;

    void trig() override;;

    bool accept(yarp::os::Things &thing) override;
    yarp::os::Things &update(yarp::os::Things &thing) override;
};

#endif
