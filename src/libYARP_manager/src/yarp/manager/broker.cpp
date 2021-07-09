/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
