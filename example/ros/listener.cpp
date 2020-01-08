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

    /* creates a node called /yarp/listener */
    Node node("/yarp/listener");

    /* subscribe to topic chatter */
    yarp::os::Subscriber<yarp::rosmsg::std_msgs::String> subscriber;
    if (!subscriber.topic("/chatter")) {
        cerr<< "Failed to subscriber to /chatter\n";
        return -1;
    }

    /* read data from the topic */
    while (true) {
        yarp::rosmsg::std_msgs::String data;
        subscriber.read(data);
        cout << "Received:" << data.data << " " << endl;
    }

    return 0;
}
