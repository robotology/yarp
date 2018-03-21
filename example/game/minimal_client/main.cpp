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

int main(int argc, char *argv[]) {
    Network yarp;

    // port name of "..." means "I don't care, just give me a free name"
    const char *name = "...";
    if (argc>=2) {
        name = argv[1];
    }

    printf("Creating port\n");
    Port port;

    printf("Registering port\n");
    port.open(name);

    printf("Connecting with game server\n");
    Network::connect(port.getName(),"/game");

    Bottle cmd;
    Bottle response;
    cmd.addString("look");
    printf("Sending command %s\n", cmd.toString().c_str());
    port.write(cmd,response);
    printf("Raw response: %s\n", response.toString().c_str());

    // pick out map part
    Bottle& map = response.findGroup("look").findGroup("map");
    printf("Here's your local view:\n");
    int i;
    for (i=1; i<map.size(); i++) {
        printf("%s\n", map.get(i).asString().c_str());
    }

    // pick out and print player information
    Bottle& players = response.findGroup("look").findGroup("players");
    for (i=1; i<players.size(); i++) {
        Bottle *player = players.get(i).asList();
        if (player!=NULL) {
            Bottle& location = player->findGroup("location");
            Value& life = player->find("life");

            ConstString playerName = player->get(0).asString();
            printf("PLAYER %s is at (%d,%d) with lifeforce %d\n", 
                   playerName.c_str(), 
                   location.get(1).asInt(),
                   location.get(2).asInt(),
                   life.asInt());
            printf("Logging out\n");
        }
    }
    port.close();

    return 0;
}
