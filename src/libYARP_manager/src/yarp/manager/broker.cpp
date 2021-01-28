/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/manager/broker.h>

using namespace yarp::manager;

//unsigned int Broker::UNIQUEID = 0;

Broker::Broker()
{
    UNIQUEID = 0;
    eventSink = nullptr;
    bWithWatchDog = false;
}

Broker::~Broker() = default;

void Broker::setEventSink(BrokerEventSink* pEventSink)
{
    eventSink = pEventSink;
}

unsigned int Broker::generateID()
{
    return UNIQUEID++;
}
