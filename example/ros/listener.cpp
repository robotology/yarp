/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Node.h>
#include <yarp/os/Subscriber.h>

#include <yarp/rosmsg/std_msgs/String.h>

using yarp::os::Network;
using yarp::os::Node;
using yarp::os::Subscriber;

namespace {
YARP_LOG_COMPONENT(LISTENER, "yarp.example.ros.listener")
}


int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;

    /* creates a node called /yarp/listener */
    Node node("/yarp/listener");

    /* subscribe to topic chatter */
    yarp::os::Subscriber<yarp::rosmsg::std_msgs::String> subscriber;
    if (!subscriber.topic("/chatter")) {
        yCError(LISTENER) << "Failed to subscriber to /chatter";
        return -1;
    }

    /* read data from the topic */
    while (true) {
        yarp::rosmsg::std_msgs::String data;
        subscriber.read(data);
        yCInfo(LISTENER) << "Received:" << data.data;
    }

    return 0;
}
