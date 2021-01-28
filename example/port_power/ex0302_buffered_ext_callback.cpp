/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
