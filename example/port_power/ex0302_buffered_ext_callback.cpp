/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

class DataProcessor : public TypedReaderCallback<Bottle> {
     virtual void onRead(Bottle& b) {
          // process data in b
         printf("Got %s\n", b.toString().c_str());
     }
};
DataProcessor processor;



int main() {
    Network yarp;
    
    DataProcessor processor;
    BufferedPort<Bottle> p;
    p.useCallback(processor);  // input should go to processor.onRead()
    p.open("/in");          // Give it a name on the network.
    while (true) {
        printf("main thread free to do whatever it wants\n");
        Time::delay(10);
    }
    
    return 0;
}
