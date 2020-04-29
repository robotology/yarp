/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/serversql/impl/Subscriber.h>

#include <yarp/os/LogComponent.h>

using yarp::serversql::impl::Subscriber;

namespace {
YARP_LOG_COMPONENT(SUBSCRIBER,
                   "yarp.serversql.impl.Subscriber",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::defaultPrintCallback(),
                   nullptr)
} // namespace

bool Subscriber::apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote)
{
    YARP_UNUSED(event);
    YARP_UNUSED(remote);

    std::string tag = cmd.get(0).asString();
    bool ok = false;
    if (tag == "subscribe" ||
        tag == "unsubscribe" ||
        tag == "announce" ||
        tag == "topic" ||
        tag == "untopic" ||
        tag == "type") {
        if (!silent) {
            yCInfo(SUBSCRIBER, "-> %s", cmd.toString().c_str());
        }
    }
    if (tag == "subscribe") {
        std::string src = cmd.get(1).asString();
        std::string dest = cmd.get(2).asString();
        std::string mode = cmd.get(3).asString();
        if (!dest.empty()) {
            ok = addSubscription(src, dest, mode);
            reply.clear();
            reply.addVocab(ok ? yarp::os::createVocab('o', 'k')
                              : yarp::os::createVocab('f', 'a', 'i', 'l'));
            return ok;
        }

        // list subscriptions
        listSubscriptions(src, reply);
        return true;
    }
    if (tag == "unsubscribe") {
        ok = removeSubscription(cmd.get(1).asString(),
                                cmd.get(2).asString());
        reply.clear();
        reply.addVocab(ok ? yarp::os::createVocab('o', 'k')
                          : yarp::os::createVocab('f', 'a', 'i', 'l'));
        return ok;
    }
    if (tag == "announce") {
        if (cmd.get(2).isInt32()) {
            welcome(cmd.get(1).asString(), cmd.get(2).asInt32() ? 1 : 0);
        } else {
            welcome(cmd.get(1).asString(), true);
        }
        reply.clear();
        reply.addVocab(yarp::os::createVocab('o', 'k'));
        return true;
    }
    if (tag == "topic") {
        if (cmd.size() >= 2) {
            bool result = setTopic(cmd.get(1).asString(),
                                   cmd.get(2).asString(),
                                   true);
            reply.clear();
            reply.addVocab(replyCode(result));
            return true;
        }

        reply.clear();
        listTopics(reply);
        return true;
    }
    if (tag == "type") {
        if (cmd.size() == 4) {
            bool result = setType(cmd.get(1).asString(),
                                  cmd.get(2).asString(),
                                  cmd.get(3).asString());
            reply.clear();
            reply.addVocab(replyCode(result));
            return true;
        }
        if (cmd.size() == 3) {
            std::string result =
                getType(cmd.get(1).asString(),
                        cmd.get(2).asString());
            reply.clear();
            if (result.empty()) {
                reply.addVocab(replyCode(false));
            } else {
                reply.addString(cmd.get(0).asString());
                reply.addString(cmd.get(1).asString());
                reply.addString(cmd.get(2).asString());
                reply.addString(result);
            }
            return true;
        }
        reply.clear();
        reply.addVocab(replyCode(false));
        return true;
    }
    if (tag == "untopic") {
        bool result = setTopic(cmd.get(1).asString(), "", false);
        reply.clear();
        reply.addVocab(replyCode(result));
        return true;
    }
    return ok;
}
