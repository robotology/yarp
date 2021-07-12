/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_NOP_NOP_H
#define YARP_NOP_NOP_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>

/**
* @ingroup dev_impl_fake
*
* \brief `test_nop`: A test device that does absolutely nothing.
*/
class Nop :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread
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

private:
    yarp::conf::float64_t period{1.0};
};

#endif // YARP_NOP_NOP_H
