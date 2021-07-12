/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/GenericVocabs.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Port;
using yarp::os::Property;
using yarp::os::Value;


int main(int argc, char* argv[])
{
    if (argc <= 1) {
        printf("This is a very simple database\n");
        printf("Call as: %s --name /database\n", argv[0]);
        printf("Then you can test it by running:\n");
        printf("  yarp rpc /database\n");
        printf("And typing things like:\n");
        printf("  set x 24\n");
        printf("  get x\n");
        printf("  get y\n");
        printf("  rm x\n");
        printf("  get x\n");
        printf("  set \"my favorite numbers\" (5 10 16)\n");
        printf("  get \"my favorite numbers\"\n");
    }

    Network yarp;

    Property option;
    option.fromCommand(argc, argv);

    Property state;

    Port port;
    port.open(option.check("name", Value("/database")).asString());

    while (true) {
        Bottle cmd;
        Bottle response;
        port.read(cmd, true); // true -> will reply

        Bottle tmp;
        tmp.add(cmd.get(1));
        std::string key = tmp.toString();

        switch (cmd.get(0).asVocab32()) {
        case VOCAB_SET:
            state.put(key, cmd.get(2));
            break;
        case VOCAB_GET:
            break;
        case VOCAB_REMOVE:
            state.unput(key);
            break;
        }
        Value& v = state.find(key);
        response.addVocab32(v.isNull() ? VOCAB_NOT : VOCAB_IS);
        response.add(cmd.get(1));
        if (!v.isNull()) {
            response.add(v);
        }
        port.reply(response);
    }

    return 0;
}
