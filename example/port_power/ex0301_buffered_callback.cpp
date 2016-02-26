/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
