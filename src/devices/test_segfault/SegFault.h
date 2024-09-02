/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SEGFAULT_SEGFAULT_H
#define YARP_SEGFAULT_SEGFAULT_H

#include <yarp/os/Thread.h>
#include <yarp/dev/DeviceDriver.h>
#include "SegFault_ParamsParser.h"

/**
* @ingroup dev_impl_fake
*
* \brief `test_segfault`: A test device that causes a segmentation fault.
*
* Parameters required by this device are shown in class: FakeIMU_ParamsParser
*/
class SegFault :
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public SegFault_ParamsParser
{
public:
    SegFault();
    SegFault(const SegFault&) = delete;
    SegFault(SegFault&&) = delete;
    SegFault& operator=(const SegFault&) = delete;
    SegFault& operator=(SegFault&&) = delete;

    ~SegFault() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    void run() override;

};

#endif // YARP_SEGFAULT_SEGFAULT_H
