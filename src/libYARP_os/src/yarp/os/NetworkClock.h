/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NETWORKCLOCK_H
#define YARP_OS_NETWORKCLOCK_H

#include <yarp/os/api.h>

#include <yarp/os/Clock.h>

#include <string>

namespace yarp {
namespace os {

class YARP_os_API NetworkClock : public Clock
{
public:
    NetworkClock();
    virtual ~NetworkClock();

    bool open(const std::string& clockSourcePortName, std::string localPortName = "");

    double now() override;
    void delay(double seconds) override;
    bool isValid() const override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_NETWORKCLOCK_H
