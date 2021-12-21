/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_COMMANDBOTTLE_H
#define YARP_OS_COMMANDBOTTLE_H

#include <yarp/os/Portable.h>
#include <yarp/os/Bottle.h>

namespace yarp::os {

/*
 * This is a class that contains both the command and the reply.
 *
 * The only advantage of this class compared to using two bottles is that
 * portmonitors are able to know, in the `updateReply` method, what was the
 * request for the reply received.
 */
class YARP_os_API CommandBottle :
    public yarp::os::Portable
{
public:
    CommandBottle() = default;
    explicit CommandBottle(const std::string& text);
    explicit CommandBottle(std::initializer_list<yarp::os::Value> values);

    CommandBottle(const CommandBottle& rhs) = default;
    CommandBottle(CommandBottle&& rhs) noexcept = default;
    CommandBottle& operator=(const CommandBottle& rhs) = default;
    CommandBottle& operator=(CommandBottle&& rhs) noexcept = default;
    ~CommandBottle() override = default;


    bool write(yarp::os::ConnectionWriter& connection) const override;
    bool read(yarp::os::ConnectionReader& connection) override;

    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;
};

} // namespace yarp::os

#endif // YARP_OS_COMMANDBOTTLE_H
