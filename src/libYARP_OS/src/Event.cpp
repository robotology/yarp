// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Event.h>
#include <yarp/os/impl/Logger.h>

#include <ace/Auto_Event.h>
#include <ace/Manual_Event.h>

using namespace yarp::os::impl;

#define EVENT_IMPL(x) (static_cast<ACE_Event*>(x))

yarp::os::Event::Event(bool autoResetAfterWait) {
    if (autoResetAfterWait) {
        implementation = new ACE_Auto_Event;
    } else {
        implementation = new ACE_Manual_Event;
    }
    YARP_ASSERT(implementation!=NULL);
}

yarp::os::Event::~Event() {
    if (implementation!=NULL) {
        delete EVENT_IMPL(implementation);
        implementation = NULL;
    }
}

void yarp::os::Event::wait() {
    EVENT_IMPL(implementation)->wait();
}

void yarp::os::Event::signal() {
    EVENT_IMPL(implementation)->signal();
}

void yarp::os::Event::reset() {
    EVENT_IMPL(implementation)->reset();
}
