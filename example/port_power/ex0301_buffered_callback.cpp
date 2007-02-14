// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    Network::init();
    
    DataPort p;
    p.useCallback();  // input should go to onRead() callback
    p.open("/in");          // Give it a name on the network.
    while (true) {
        printf("main thread free to do whatever it wants\n");
        Time::delay(10);
    }
    
    Network::fini();
    return 0;
}
