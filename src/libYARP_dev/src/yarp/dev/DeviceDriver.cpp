/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
    YARP_LOG_COMPONENT(DEVICERESPONDER, "yarp.dev.DeviceResponder")
}

DeviceResponder::DeviceResponder() {
    makeUsage();
}

void DeviceResponder::addUsage(const char *txt, const char *explain) {
    examples.addString(txt); //Value::makeList(txt));
    explains.addString((explain!=nullptr)?explain:"");
    details.add(Value::makeList(txt));
    std::string more = std::string("   ") + ((explain != nullptr) ? explain : "");
    details.addString(more.c_str());
}


void DeviceResponder::addUsage(const Bottle& bot, const char *explain) {
    addUsage(bot.toString().c_str(),explain);
}


bool DeviceResponder::respond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab32()) {
    case yarp::os::createVocab32('h','e','l','p'):
        if (examples.size()>=1) {
            reply.add(Value::makeVocab32("many"));
            if (command.get(1).toString()=="more") {
                reply.append(details);
            } else {
                reply.append(examples);
            }
            return true;
        } else {
            reply.addString("no documentation available");
            return false;
        }
        break;
    default:
        reply.addString("command not recognized");
        return false;
    }
    return false;
}

bool DeviceResponder::read(ConnectionReader& connection)
{
    Bottle cmd;
    Bottle response;
    if (!cmd.read(connection)) {
        return false;
    }
    yCTrace(DEVICERESPONDER, "Command received: %s", cmd.toString().c_str());
    respond(cmd, response);
    if (response.size() >= 1) {
        ConnectionWriter* writer = connection.getWriter();
        if (writer != nullptr) {
            if (response.get(0).toString() == "many" && writer->isTextMode()) {
                for (size_t i = 1; i < response.size(); i++) {
                    Value& v = response.get(i);
                    if (v.isList()) {
                        v.asList()->write(*writer);
                    } else {
                        Bottle b;
                        b.add(v);
                        b.write(*writer);
                    }
                }
            } else {
                response.write(*writer);
            }

            yCTrace(DEVICERESPONDER, "Response sent: %s", response.toString().c_str());
        }
    } else {
        ConnectionWriter* writer = connection.getWriter();
        if (writer != nullptr) {
            response.clear();
            response.addVocab32("nak");
            response.write(*writer);
        }
    }
    return true;
}


void DeviceResponder::makeUsage() {
    examples.clear();
    explains.clear();
    details.clear();
    addUsage("[help]", "list usage");
    addUsage("[help] [more]", "list usage with some comments");
}
