// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

class DataProcessor : public PortReader {
     virtual bool read(ConnectionReader& connection) {
          Bottle b;
          b.read(connection);
          // process data in b
          printf("Got %s\n", b.toString().c_str());      
          return true;
     }
};
DataProcessor processor;

int main() {
    Network yarp;
    
    Port p;            // Create a port.
    p.open("/in");     // Give it a name on the network.
    p.setReader(processor);  // no need to call p.read() on port any more.

    while (true) {
        printf("main thread free to do whatever it wants\n");
        Time::delay(10);
    }
    
    return 0;
}
