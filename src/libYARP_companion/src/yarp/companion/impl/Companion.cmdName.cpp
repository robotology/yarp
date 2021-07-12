/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/ContactStyle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/NameServer.h>


using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Contact;
using yarp::os::ContactStyle;
using yarp::os::NetworkBase;
using yarp::os::Value;
using yarp::os::impl::NameServer;

int Companion::cmdName(int argc, char* argv[])
{
    ContactStyle style;
    style.quiet = true;
    Bottle cmd;
    Bottle reply;
    for (int i = 0; i < argc; i++) {
        Value v;
        v.fromString(argv[i]);
        cmd.add(v);
    }

    std::string key = cmd.get(0).asString();
    if (key == "query") {
        Contact result = NetworkBase::queryName(cmd.get(1).asString());
        if (!result.isValid()) {
            yCError(COMPANION, "%s not known.", cmd.get(1).asString().c_str());
            return 1;
        }
        std::string txt = NameServer::textify(result);
        yCInfo(COMPANION, "%s", txt.c_str());
        return 0;
    }
    if (key == "register") {
        std::string portName = cmd.get(1).asString();
        std::string machine = "...";
        std::string carrier = "...";
        int port = 0;
        bool spec = false;
        if (cmd.size() > 2) {
            carrier = cmd.get(2).asString();
            spec = true;
        }
        if (cmd.size() > 3) {
            machine = cmd.get(3).asString();
        }
        if (cmd.size() > 4) {
            if (!cmd.get(4).isInt32()) {
                port = 0;
            } else {
                port = cmd.get(4).asInt32();
            }
        }
        Contact result;
        if (spec) {
            result = NetworkBase::registerContact(Contact(portName, carrier, machine, port));
        } else {
            result = NetworkBase::registerName(portName);
        }
        std::string txt = NameServer::textify(result);
        yCInfo(COMPANION, "%s", txt.c_str());
        return 0;
    }
    if (key == "unregister") {
        std::string portName = cmd.get(1).asString();
        Contact result;
        result = NetworkBase::unregisterName(portName);
        yCInfo(COMPANION, "Unregistered name.");
        return 0;
    }


    bool ok = NetworkBase::writeToNameServer(cmd,
                                             reply,
                                             style);
    if (!ok) {
        yCError(COMPANION, "Failed to reach name server");
        return 1;
    }
    if (reply.size() == 1 && reply.get(0).isString()) {
        yCInfo(COMPANION) << reply.get(0).asString().c_str();
    } else if (reply.get(0).isVocab32() && reply.get(0).asVocab32() == yarp::os::createVocab32('m', 'a', 'n', 'y')) {
        for (size_t i = 1; i < reply.size(); i++) {
            Value& v = reply.get(i);
            if (v.isString()) {
                yCInfo(COMPANION) << " " << v.asString().c_str();
            } else {
                yCInfo(COMPANION) << " " << v.toString().c_str();
            }
        }
    } else {
        yCInfo(COMPANION, "%s", reply.toString().c_str());
    }
    return 0;
}
