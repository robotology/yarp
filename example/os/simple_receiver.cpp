// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Network::init();
    Bottle bot;
    Port input;
    input.open("/receiver");
    // usually, we create connections externally, but just for this example...
    Network::connect("/sender","/receiver");
    input.read(bot);
    printf("Got message: %s\n", bot.toString().c_str());
    input.close();
    Network::fini();
    return 0;
}
