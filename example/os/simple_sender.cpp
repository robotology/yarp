// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Network::init();
    Port output;
    output.open("/sender");
    int top = 100;
    for (int i=1; i<=top; i++) {
        // prepare a message
        Bottle bot; 
        bot.addString("testing");
        bot.addInt(i);
        bot.addString("of");
        bot.addInt(top);
        // send the message
        output.write(bot);
        printf("Sent message: %s\n", bot.toString().c_str());
        // wait a while
        Time::delay(1);
    }
    output.close();
    Network::fini();
    return 0;
}
