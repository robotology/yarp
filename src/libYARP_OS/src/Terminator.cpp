/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>
#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/PortCommand.h>

#include <yarp/os/Terminator.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Vocab.h>


using namespace yarp::os::impl;
using namespace yarp::os;

bool Terminator::terminateByName(const char *name) {
    if (name == nullptr)
        return false;

    ConstString s(name);

    if (s.find("/quit")==ConstString::npos) {
        // name doesn't include /quit
        // old mechanism won't work, let's try new
        PortCommand pc('\0', "i");
        Companion::sendMessage(s, pc, true);
        return true;
    }

    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    Bottle cmd("quit"), reply;
    Contact c = NetworkBase::queryName(s.c_str());
    if (!c.isValid()) {
        fprintf(stderr, "Terminator port not found\n");
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

Terminee::Terminee(const char *name) {
    ok = false;
    implementation = nullptr;
    if (name == nullptr) {
        quit = true;
        printf("Terminator: Please supply a proper port name\n");
        return;
    }

    ConstString s(name);
    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    implementation = new TermineeHelper();
    yAssert(implementation!=nullptr);
    TermineeHelper& helper = HELPER(implementation);
    ok = helper.open(s.c_str());
    if (!ok) {
        quit = true;
        fprintf(stderr, "Kill port conflict: make sure you supply a distinct --name /PORTNAME\n");
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

Terminee::~Terminee() {
    TermineeHelper& helper = HELPER(implementation);
    if (!quit) {
        Terminator::terminateByName(helper.getName().c_str());
    }

    stop();

    if (implementation!=nullptr) {
        delete &HELPER(implementation);
    }
}


void Terminee::run() {
    TermineeHelper& helper = HELPER(implementation);
    while (!isStopping() && !quit) {
        Bottle cmd, reply;
        bool ok = helper.read(cmd, true);
		if (!ok) {
			continue;
		}
        if (cmd.get(0).asString()=="quit") {
            quit = true;
            reply.addVocab(VOCAB2('o', 'k'));
        } else {
            reply.addVocab(VOCAB4('h', 'u', 'h', '?'));
        }
        helper.reply(reply);
    }
}
