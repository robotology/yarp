/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/NameConfig.h>

#include <algorithm>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::NetworkBase;
using yarp::os::Value;
using yarp::os::impl::NameConfig;


int Companion::cmdEmergency(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yCInfo(COMPANION);
    yCInfo(COMPANION, " --- EMERGENCY MEETING ---");
    yCInfo(COMPANION);

    NameConfig nc;
    std::string name = nc.getNamespace();
    Bottle msg;
    Bottle reply;
    msg.addString("bot");
    msg.addString("list");
    NetworkBase::write(name, msg, reply);

    msg.clear();
    msg.addVocab(yarp::os::createVocab('e', 'm', 'e', 'r'));
    Bottle& ports = msg.addList();
    std::map<size_t, size_t> votes;

    for (size_t i = 1 /* 0 is the string "ports" */; i < reply.size(); i++) {
        Bottle* entry = reply.get(i).asList();
        if (entry != nullptr) {
            std::string port = entry->check("name", Value("")).asString();
            if (!port.empty() && port != "fallback") {
                ports.addString(port);
                votes[ports.size() - 1] = 0;
            }
        }
    }

    if (ports.size() < 3) {
        yCInfo(COMPANION, "No one was ejected. (Skipped)");
        return -1;
    }

    for (size_t i = 0; i < ports.size(); i++) {
        auto port = ports.get(i).asString();
        NetworkBase::write(port, msg, reply, true);
        yCInfo(COMPANION, "<%s> %s", ports.get(i).asString().c_str(), reply.get(1).asString().c_str());

        size_t index = reply.get(0).asInt32();
        votes[index]++;
    }
    yCInfo(COMPANION);

    auto max = std::max_element(votes.begin(),
                                votes.end(),
                                [] (const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) -> bool { return a.second < b.second; } );
    auto rmax = std::max_element(votes.rbegin(),
                                votes.rend(),
                                [] (const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) -> bool { return a.second < b.second; } );

    if (max->first != rmax->first) {
        yCInfo(COMPANION, "No one was ejected. (Tie)");
        yCInfo(COMPANION);
        return -1;
    }

    auto impostor = ports.get(max->first).asString();
    NetworkBase::unregisterName(impostor);
    yCInfo(COMPANION, "%s was ejected.", impostor.c_str());
    yCInfo(COMPANION);
    yCInfo(COMPANION, "%s was not The Impostor.", impostor.c_str());
    yCInfo(COMPANION);

    return 0;
}
