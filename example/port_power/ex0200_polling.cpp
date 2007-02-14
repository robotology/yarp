// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network::init();
    

    BufferedPort<Bottle> p; // Create a port.
    p.open("/in");          // Give it a name on the network.
    while (true) {
        Bottle *b = p.read(false); // Read from the port.  Don't wait
        if (b!=NULL) {
            printf("Got %s\n", b->toString().c_str());
        } else {
            printf("No data yet...\n");
            Time::delay(0.5);
        }
    }
    
    Network::fini();
    return 0;
}
