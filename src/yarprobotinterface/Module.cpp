/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Debug.h"
#include "Module.h"
#include "RobotInterface.h"
#include "XMLReader.h"

#include <yarp/os/ResourceFinder.h>

class RobotInterface::Module::Private
{
public:
    Private(Module *parent);
    ~Private();

    Module * const parent;
    RobotInterface::Robot robot;
};


RobotInterface::Module::Private::Private(Module *parent) :
    parent(parent)
{

}


RobotInterface::Module::Private::~Private()
{

}

RobotInterface::Module::Module() :
    mPriv(new Private(this))
{
}

RobotInterface::Module::~Module()
{
    delete mPriv;
}

double RobotInterface::Module::getPeriod()
{
    return 2;
}

bool RobotInterface::Module::open(yarp::os::Searchable& config)
{
    RobotInterface::XMLReader reader;
    mPriv->robot = reader.getRobot("/opt/iit/src/robotInterface/icub_torso.xml");
    debug() << mPriv->robot;

    return mPriv->robot.enterPhase(RobotInterface::ActionPhaseStartup);
}

bool RobotInterface::Module::interruptModule()
{
    return mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt);
}

bool RobotInterface::Module::close()
{
    return mPriv->robot.enterPhase(RobotInterface::ActionPhaseShutdown);
}
