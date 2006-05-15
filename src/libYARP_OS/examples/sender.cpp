// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
// source for sender.cpp
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Bottle bot1; 
    bot1.addString("testing"); // a simple message
    Port output;
    output.open("/out");
    for (int i=0; i<100; i++) {
        output.write(bot1);
        printf("Sent message: %s\n", bot1.toString().c_str());
        bot1.addInt(i); // change the message for next time
        Time::delay(1);
    }
    output.close();
    return 0;
}
