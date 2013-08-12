// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Giorgio Metta, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/PlatformStdio.h>
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
    if (name == NULL)
        return false;

    String s(name);

    if (YARP_STRSTR(s,"/quit")==String::npos) {
        // name doesn't include /quit
        // old mechanism won't work, let's try new
        PortCommand pc('\0',"i");
        Companion::sendMessage(s,pc,true);
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
        fprintf(stderr,"Terminator port not found\n");
        return false;
    }
    ContactStyle style;
    style.quiet = false;
    style.carrier = "text_ack";
    style.expectReply = true;
    NetworkBase::write(c,cmd,reply,style);

    return true;
}

#define TermineeHelper Port
#define HELPER(x) (*((TermineeHelper*)(x)))

Terminee::Terminee(const char *name) {
    ok = false;
    if (name == NULL) {
        quit = true;
        ACE_OS::printf("Terminator: Please supply a proper port name\n");
        return;
    }

    String s(name);
    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }
    
    implementation = new TermineeHelper();
    YARP_ASSERT(implementation!=NULL);
    TermineeHelper& helper = HELPER(implementation);
    ok = helper.open(s.c_str());
    if (!ok) {
        quit = true;
        fprintf(stderr,"Kill port conflict: make sure you supply a distinct --name /PORTNAME\n");
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

    if (implementation!=NULL) {
        delete &HELPER(implementation);
    }
}


void Terminee::run() {
    TermineeHelper& helper = HELPER(implementation);
    while (!isStopping() && !quit) {
        Bottle cmd, reply;
        bool ok = helper.read(cmd,true);
		if (!ok) {
			continue;
		}
        if (cmd.get(0).asString()=="quit") {
            quit = true;
            reply.addVocab(VOCAB2('o','k'));
        } else {
            reply.addVocab(VOCAB4('h','u','h','?'));
        }
        helper.reply(reply);
    }
}
