/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "RobotInterface.h"


class RobotInterface::RobotInterface::Private
{
public:
    Private(RobotInterface *parent);
    ~Private();

    RobotInterface * const parent;
};


RobotInterface::RobotInterface::Private::Private(RobotInterface *parent) :
    parent(parent)
{

}


RobotInterface::RobotInterface::Private::~Private()
{

}

RobotInterface::RobotInterface::RobotInterface() :
    mPriv(new Private(this))
{

}

RobotInterface::RobotInterface::~RobotInterface()
{
    delete mPriv;
}
