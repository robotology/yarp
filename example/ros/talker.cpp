/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <iostream>
#include <yarp/os/all.h>

using namespace yarp::os;
using namespace std;

/* Make sure you run yarpidl_rosmsg std_msg/String */
/* to generate String.h  */
#include <yarp/rosmsg/std_msgs/String.h>

int main(int argc, char *argv[]) {
    Network yarp;

    /* creates a node called /yarp/talker */
    Node node("/yarp/talker");

    /* subscribe to topic chatter */
    yarp::os::Publisher<yarp::rosmsg::std_msgs::String> publisher;
    if (!publisher.topic("/chatter")) {
        cerr<< "Failed to create publisher to /chatter\n";
        return -1;
    }

    while (true) {
        /* prepare some data */
        yarp::rosmsg::std_msgs::String data;
        data.data="Hello from YARP";

        /* publish it to the topic */
        publisher.write(data);

        /* wait some time to avoid flooding with messages */
        Time::delay(0.1);
    }

    return 0;
}
