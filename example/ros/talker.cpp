/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Time.h>

#include <yarp/rosmsg/std_msgs/String.h>

using yarp::os::Network;
using yarp::os::Node;
using yarp::os::Publisher;

namespace {
YARP_LOG_COMPONENT(TALKER, "yarp.example.ros.talker")
constexpr double loop_delay = 0.1;
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;

    /* creates a node called /yarp/talker */
    Node node("/yarp/talker");

    /* subscribe to topic chatter */
    yarp::os::Publisher<yarp::rosmsg::std_msgs::String> publisher;
    if (!publisher.topic("/chatter")) {
        yCError(TALKER) << "Failed to create publisher to /chatter";
        return -1;
    }

    while (true) {
        /* prepare some data */
        yarp::rosmsg::std_msgs::String data;
        data.data = "Hello from YARP";

        /* publish it to the topic */
        publisher.write(data);

        /* wait some time to avoid flooding with messages */
        yarp::os::Time::delay(loop_delay);
    }

    return 0;
}
