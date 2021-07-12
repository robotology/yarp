/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_SYSTEMCLOCK_H
#define YARP_OS_SYSTEMCLOCK_H

#include <yarp/os/Clock.h>

namespace yarp {
namespace os {

class YARP_os_API SystemClock : public Clock
{
public:
    double now() override;
    void delay(double seconds) override;

    bool isValid() const override;

    static double nowSystem();
    static void delaySystem(double seconds);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_SYSTEMCLOCK_H
