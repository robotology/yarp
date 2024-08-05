/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_NOP_NOP_H
#define YARP_NOP_NOP_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include "Nop_ParamsParser.h"

/**
* @ingroup dev_impl_fake
*
* \brief `test_nop`: A test device that does absolutely nothing.
*
* * Parameters required by this device are shown in class: FakeIMU_ParamsParser
*/
class Nop :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public Nop_ParamsParser
{
public:
    Nop();
    Nop(const Nop&) = delete;
    Nop(Nop&&) = delete;
    Nop& operator=(const Nop&) = delete;
    Nop& operator=(Nop&&) = delete;

    ~Nop() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    void run() override;

};

#endif // YARP_NOP_NOP_H
