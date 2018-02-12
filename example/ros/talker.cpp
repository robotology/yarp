/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>
#include <yarp/os/all.h>

using namespace yarp::os;
using namespace std;

/* Make sure you run yarpidl_rosmsg std_msg/String */
/* to generate String.h  */
#include "String.h"

int main(int argc, char *argv[]) {
    Network yarp;
    
    /* creates a node called /yarp/talker */
    Node node("/yarp/talker");
    
    /* subscribe to topic chatter */
    yarp::os::Publisher<String> publisher;
    if (!publisher.topic("/chatter")) {
        cerr<< "Failed to create publisher to /chatter\n";
        return -1;
    }

    while (true) {
        /* prepare some data */
        String data;
        data.data="Hello from YARP";

        /* publish it to the topic */
        publisher.write(data);

        /* wait some time to avoid flooding with messages */
        Time::delay(0.1);
    }
  
    return 0;
}
