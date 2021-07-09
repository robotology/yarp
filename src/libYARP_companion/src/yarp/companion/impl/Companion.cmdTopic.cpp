/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::NetworkBase;

int Companion::cmdTopic(int argc, char *argv[])
{
    int mode = 1;
    if (argc>=1) {
        if (std::string(argv[0]) == "--remove") {
            mode = -1;
            argc--;
            argv++;
        }
        if (std::string(argv[0]) == "--list") {
            Bottle cmd, reply;
            cmd.addString("topic");
            bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                         cmd,
                                         reply,
                                         false,
                                         true);
            if (!ok) {
                yCError(COMPANION, "Failed to read topic list");
                return 1;
            }
            if (reply.size()==0) {
                yCInfo(COMPANION, "No topics");
            } else {
                yCInfo(COMPANION, "Topics: %s", reply.toString().c_str());
            }
            return 0;
        }
    }
    if (argc<1)
    {
        yCError(COMPANION, "Please supply the topic name");
        yCError(COMPANION, "(Or: '--list' to list all topics)");
        yCError(COMPANION, "(Or: '--remove <topic>' to remove a topic)");
        return 1;
    }

    Bottle cmd, reply;
    std::string act = (mode==1)?"create":"delete";
    cmd.addString((mode==1)?"topic":"untopic");
    if (std::string(argv[0])!="--list") {
        for (int i=0; i<argc; i++) {
            cmd.addString(argv[i]);
        }
    }
    bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply,
                                 false,
                                 true);
    if (ok) {
        ok = reply.get(0).asVocab32()==yarp::os::createVocab32('o', 'k');
    }
    if (!ok) {
        yCError(COMPANION,
                "Failed to %s topic %s:\n  %s",
                act.c_str(), argv[0], reply.toString().c_str());
    } else {
        yCInfo(COMPANION,
               "Topic %s %sd",
               argv[0], act.c_str());
    }

    return ok?0:1;
}
