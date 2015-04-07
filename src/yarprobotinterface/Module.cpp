/*
 * Copyright (C) 2012, 2015  iCub Facility, Istituto Italiano di Tecnologia
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
    int interruptReceived;
};


RobotInterface::Module::Private::Private(Module *parent) :
    parent(parent),
    interruptReceived(0)
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
    if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseStartup) ||
        !mPriv->robot.enterPhase(RobotInterface::ActionPhaseRun)) {
        yError() << "Error in" << ActionPhaseToString(mPriv->robot.currentPhase()) << "phase... see previous messages for more info";
        // stopModule() calls interruptModule() and then close()
        // internally. This ensure that interrupt1 phase actions
        // (i.e. detach) are performed before destroying the devices.
        stopModule();
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
    mPriv->interruptReceived++;

    yWarning() << "Interrupt #" << mPriv->interruptReceived << "# received.";

    mPriv->robot.interrupt();

    // In the first interrupt, after sending the interrupt() command
    // to the robot we exit the callback. In the close() method then
    // we proceed with the interupt1 phase, where we wait for all the
    // threads already started are joined, and finally we start the
    // interrupt1 actions.
    // In the second and third interrupts, we enter the interrupt2
    // phase in the callback. This means that in Robot, we cannot
    // wait for the other threads using join().
    switch (mPriv->interruptReceived) {
    case 1:
        break;
    case 2:
        if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt2)) {
            yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseInterrupt2) << "phase... see previous messages for more info";
            return false;
        }
        break;
    case 3:
        if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt3)) {
            yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseInterrupt3) << "phase... see previous messages for more info";
            return false;
        }
        break;
    default:
        return false;
    }

    return true;
}

bool RobotInterface::Module::close()
{
    // If called from the first interrupt, enter the corresponding
    // interrupt phase.
    switch (mPriv->interruptReceived) {
    case 1:
        if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt1)) {
            yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseInterrupt1) << "phase... see previous messages for more info";
            return false;
        }
        break;
    case 2:
    case 3:
        break;
    default:
        return false;
    }

    // Finally call the shutdown phase.
    if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseShutdown)) {
        yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseShutdown) << "phase... see previous messages for more info";
        return false;
    }

    return true;
}
