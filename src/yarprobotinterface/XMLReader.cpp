/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "XMLReader.h"


class RobotInterface::XMLReader::Private
{
public:
    Private(XMLReader *parent);
    ~Private();

    XMLReader * const parent;
};


RobotInterface::XMLReader::Private::Private(XMLReader *parent) :
    parent(parent)
{

}


RobotInterface::XMLReader::Private::~Private()
{

}

RobotInterface::XMLReader::XMLReader() :
    mPriv(new Private(this))
{

}

RobotInterface::XMLReader::~XMLReader()
{
    delete mPriv;
}
