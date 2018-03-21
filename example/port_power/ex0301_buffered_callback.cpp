/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

class DataPort : public BufferedPort<Bottle> {
    virtual void onRead(Bottle& b) {
        // process data in b
        printf("Got %s\n", b.toString().c_str());
     }
};


int main() {
    Network yarp;
    
    DataPort p;
    p.useCallback();  // input should go to onRead() callback
    p.open("/in");          // Give it a name on the network.
    while (true) {
        printf("main thread free to do whatever it wants\n");
        Time::delay(10);
    }
    
    return 0;
}
