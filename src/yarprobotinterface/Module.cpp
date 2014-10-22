/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Action.h"
#include "Device.h"
#include "Module.h"
#include "Param.h"
#include "Robot.h"
#include "XMLReader.h"

#include <yarp/os/LogStream.h>

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

bool RobotInterface::Module::configure(yarp::os::ResourceFinder &rf)
{
    if (!rf.check("config")) {
        yFatal() << "Missing \"config\" argument";
    }

    const yarp::os::ConstString &filename = rf.findFile("config");
    yTrace() << "Reading robot config file" << filename;

    RobotInterface::XMLReader reader;
    mPriv->robot = reader.getRobot(filename.c_str());
    // yDebug() << mPriv->robot;

    // User can use YARP_PORT_PREFIX environment variable to override
    // the default name, so we don't care of handling the --name
    // argument
    setName(mPriv->robot.portprefix().c_str());

    // Enter startup phase
    if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseStartup)) {
        yError() << "Error in startup phase... see previous messages for more info";
        return false;
    }

    return true;
}

double RobotInterface::Module::getPeriod()
{
    return 60;
}

bool RobotInterface::Module::updateModule()
{
    yDebug() << "robot-interface running happily";
    return true;
}

bool RobotInterface::Module::interruptModule()
{
    static int ct = 0;
    ct++;

    yWarning() << "Interrupt #" << ct << "# received.";

    switch (ct) {
    case 1:
        return mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt1);
    case 2:
        return mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt2);
    case 3:
        return mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt3);
    default:
        return false;
    }
}

bool RobotInterface::Module::close()
{
    return mPriv->robot.enterPhase(RobotInterface::ActionPhaseShutdown);
}
