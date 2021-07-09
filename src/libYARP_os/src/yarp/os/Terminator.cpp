/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/SocketTwoWayStream.h>

#include <cstdio>


using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(TERMINATOR, "yarp.os.Terminator")
}


bool Terminator::terminateByName(const char* name)
{
    if (name == nullptr) {
        return false;
    }

    std::string s(name);

    if (s.find("/quit") == std::string::npos) {
        // name doesn't include /quit
        // old mechanism won't work, let's try new
        PortCommand pc('\0', "i");
        NetworkBase::sendMessage(s, pc, true);
        return true;
    }

    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    Bottle cmd("quit");
    Bottle reply;
    Contact c = NetworkBase::queryName(s);
    if (!c.isValid()) {
        yCError(TERMINATOR, "Terminator port not found");
        return false;
    }
    ContactStyle style;
    style.quiet = false;
    style.carrier = "text_ack";
    style.expectReply = true;
    NetworkBase::write(c, cmd, reply, style);

    return true;
}

#define TermineeHelper Port
#define HELPER(x) (*((TermineeHelper*)(x)))

Terminee::Terminee(const char* name)
{
    ok = false;
    implementation = nullptr;
    if (name == nullptr) {
        quit = true;
        yCError(TERMINATOR, "Terminator: Please supply a proper port name");
        return;
    }

    std::string s(name);
    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    implementation = new TermineeHelper();
    yCAssert(TERMINATOR, implementation != nullptr);
    TermineeHelper& helper = HELPER(implementation);
    ok = helper.open(s);
    if (!ok) {
        quit = true;
        yCError(TERMINATOR, "Kill port conflict: make sure you supply a distinct --name /PORTNAME");
    } else {
        quit = false;
        start();
    }
}

void Terminee::onStop()
{
    TermineeHelper& helper = HELPER(implementation);
    quit = true;
    helper.interrupt();
}

Terminee::~Terminee()
{
    TermineeHelper& helper = HELPER(implementation);
    if (!quit) {
        Terminator::terminateByName(helper.getName().c_str());
    }

    stop();

    if (implementation != nullptr) {
        delete &HELPER(implementation);
    }
}


void Terminee::run()
{
    TermineeHelper& helper = HELPER(implementation);
    while (!isStopping() && !quit) {
        Bottle cmd;
        Bottle reply;
        bool ok = helper.read(cmd, true);
        if (!ok) {
            continue;
        }
        if (cmd.get(0).asString() == "quit") {
            quit = true;
            reply.addVocab32('o', 'k');
        } else {
            reply.addVocab32('h', 'u', 'h', '?');
        }
        helper.reply(reply);
    }
}

bool Terminee::waitQuit() const
{
    // not yet implemented
    return false;
}

bool Terminee::mustQuit() const
{
    return quit;
}


bool Terminee::isOk() const
{
    return ok;
}
