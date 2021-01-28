/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
