/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Value.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Carriers;
using yarp::os::NetworkBase;
using yarp::os::Value;


namespace {

std::string slashify(const std::string& src)
{
    if (src.length()>0) {
        if (src[0] == '/') {
            return src;
        }
    }
    return std::string("/") + src;
}

} // namespace


/**
 * Request that an output port connect to an input port.
 * @param src the name of an output port
 * @param dest the name of an input port
 * @param silent whether to print comments on the result
 * @return 0 on success, non-zero on failure
 */
int Companion::connect(const char *src, const char *dest, bool silent)
{
    bool ok = NetworkBase::connect(src, dest, nullptr, silent);
    return ok ? 0 : 1;
}


int Companion::subscribe(const char *src, const char *dest, const char *mode)
{
    Bottle cmd;
    Bottle reply;
    cmd.addString("subscribe");
    if (src != nullptr) { cmd.addString(src); }
    if (dest != nullptr) { cmd.addString(dest); }
    if (mode != nullptr) { cmd.addString(mode); }
    bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply);
    bool fail = reply.get(0).toString()=="fail";
    if (fail) {
        yCInfo(COMPANION, "Persistent connection operation failed.");
        return 1;
    }
    if (reply.get(0).toString()=="subscriptions") {
        Bottle subs = reply.tail();
        for (size_t i=0; i<subs.size(); i++) {
            Bottle *b = subs.get(i).asList();
            if (b != nullptr) {
                //Bottle& topic = b->findGroup("topic");
                const char *srcTopic = "";
                const char *destTopic = "";
                //if (topic.get(1).asInt32()) srcTopic=" (topic)";
                //if (topic.get(2).asInt32()) destTopic=" (topic)";
                yCInfo(COMPANION,
                       "Persistent connection %s%s -> %s%s",
                       b->check("src", Value("?")).asString().c_str(),
                       srcTopic,
                       b->check("dest", Value("?")).asString().c_str(),
                       destTopic);
                std::string mode = b->check("mode", Value("")).asString();
                if (!mode.empty()) {
                    yCInfo(COMPANION, " [%s]", mode.c_str());
                }
                std::string carrier = b->check("carrier", Value("")).asString();
                if (!carrier.empty()) {
                    yCInfo(COMPANION, " (%s)", carrier.c_str());
                }
                yCInfo(COMPANION);
            }
        }
        if (subs.size()==0) {
            yCInfo(COMPANION, "No persistent connections.");
        }
    } else if (ok&&reply.get(0).toString()!="ok") {
        yCInfo(COMPANION, "This name server does not support persistent connections.");
    }
    return 0;
}


int Companion::cmdConnect(int argc, char *argv[])
{
    bool persist = false;
    const char *mode = nullptr;
    if (argc>0) {
        std::string arg = argv[0];
        if (arg=="--persist") {
            persist = true;
        } else if (arg=="--persist-from") {
            persist = true;
            mode = "from";
        } else if (arg=="--persist-to") {
            persist = true;
            mode = "to";
        } else if (arg=="--list-carriers") {
            Bottle lst = Carriers::listCarriers();
            for (size_t i=0; i<lst.size(); i++) {
                yCInfo(COMPANION, "%s%s", (i>0)?" ":"", lst.get(i).asString().c_str());
            }
            yCInfo(COMPANION);
            return 0;
        } else if (arg=="--help") {
            yCInfo(COMPANION, "USAGE:");
            yCInfo(COMPANION, "yarp connect OUTPUT_PORT INPUT_PORT");
            yCInfo(COMPANION, "yarp connect OUTPUT_PORT INPUT_PORT CARRIER");
            yCInfo(COMPANION, "  Make a connection between two ports, which must both exist at the time the");
            yCInfo(COMPANION, "  connection is requested.  The connection will be terminated when either");
            yCInfo(COMPANION, "  port is closed.");
            yCInfo(COMPANION);
            yCInfo(COMPANION, "yarp connect --persist OUTPUT_PORT INPUT_PORT");
            yCInfo(COMPANION, "yarp connect --persist OUTPUT_PORT INPUT_PORT CARRIER");
            yCInfo(COMPANION, "  Ask the name server to make connections whenever the named ports are available.");
            yCInfo(COMPANION);
            yCInfo(COMPANION, "yarp connect --persist-from OUTPUT_PORT INPUT_PORT");
            yCInfo(COMPANION, "  Ask the name server to connect the OUTPUT_PORT, which must");
            yCInfo(COMPANION, "  exist at the time the connection is requested, and the INPUT_PORT");
            yCInfo(COMPANION, "  whenever it is available. The request expires when OUTPUT_PORT is closed.");
            yCInfo(COMPANION);
            yCInfo(COMPANION, "yarp connect --persist-to OUTPUT_PORT INPUT_PORT");
            yCInfo(COMPANION, "  Ask the name server to connect the OUTPUT_PORT whenever available to the");
            yCInfo(COMPANION, "  INPUT_PORT which exists at the time the connection is requested.  The ");
            yCInfo(COMPANION, "  request expires when INPUT_PORT is closed.");
            yCInfo(COMPANION, "yarp connect --list-carriers");
            yCInfo(COMPANION, "  List carriers available for connections.");
            return 0;
        }
        if (persist) {
            argv++;
            argc--;
        }
    } else {
        yCError(COMPANION, "[get help with 'yarp connect --help']");
    }
    if (argc<2||argc>3) {
        if (persist&&argc<2) {
            if (argc==0) {
                return subscribe(nullptr, nullptr);
            }
            return subscribe(argv[0], nullptr);

        }
        if (argc<2) {
            if (argc==0) {
                return subscribe(nullptr, nullptr);
            }
            int result = ping(argv[0], true);
            int result2 = subscribe(argv[0], nullptr);
            return (result==0)?result2:result;
        }
        yCError(COMPANION, "Currently must have two/three arguments, a sender port and receiver port (and an optional protocol)");
        return 1;
    }

    const char *src = argv[0];
    std::string dest = argv[1];
    if (argc>=3) {
        const char *proto = argv[2];
        dest = std::string(proto) + ":/" + slashify(dest);
    }

    if (persist) {
        return subscribe(src, dest.c_str(), mode);
    }

    return connect(src, dest.c_str(), false);
}
