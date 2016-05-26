/*
 * Copyright (C) 2016 iCub Facility
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
    
    /* creates a node called /yarp/listener */
    Node node("/yarp/listener");
    
    /* subscribe to topic chatter */
    yarp::os::Subscriber<String> subscriber;
    if (!subscriber.topic("/chatter")) {
        cerr<< "Failed to subscriber to /chatter\n";
        return -1;
    }

    /* read data from the topic */
    while (true) {
        String data;
        subscriber.read(data);
        cout << "Received:" << data.data << " " << endl;
    }
  
    return 0;
}
