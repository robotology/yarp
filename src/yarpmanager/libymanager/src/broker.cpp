/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "broker.h"

//unsigned int Broker::UNIQUEID = 0;

Broker::Broker()
{
    UNIQUEID = 0;
    eventSink = NULL;
    bWithWatchDog = false;
}

Broker::~Broker()
{
}

void Broker::setEventSink(BrokerEventSink* pEventSink)
{
    eventSink = pEventSink;
}

unsigned int Broker::generateID(void)
{
    return UNIQUEID++;
}
