/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Action.h"
#include "Device.h"
#include "Module.h"
#include "Param.h"
#include "Robot.h"
#include "XMLReader.h"

#include <yarp/conf/system.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcServer.h>

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__)
#  include <csignal>
#  include <cstring>
#  include <execinfo.h>
#endif

class RobotInterface::Module::Private
{
public:
    Private(Module *parent);
    ~Private();

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__)
    static struct sigaction old_action;
    static void sigsegv_handler(int nSignum, siginfo_t* si, void* vcontext);
#endif

    Module * const parent;
    RobotInterface::Robot robot;
    int interruptReceived;
    yarp::os::RpcServer rpcPort;
    bool closed;
    bool closeOk;
};

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__)
struct sigaction RobotInterface::Module::Private::old_action;
#endif

RobotInterface::Module::Private::Private(Module *parent) :
    parent(parent),
    interruptReceived(0),
    closed(false),
    closeOk(true)
{
}

RobotInterface::Module::Private::~Private() = default;

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__)
void RobotInterface::Module::Private::sigsegv_handler(int nSignum, siginfo_t* si, void* vcontext)
{
    auto context = reinterpret_cast<ucontext_t*>(vcontext);
    context->uc_mcontext.gregs[REG_RIP]++;

    const size_t max_depth = 100;
    size_t stack_depth;
    void* stack_addrs[max_depth];
    char** stack_strings;
    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    yError("yarprobotinterface intercepted a segmentation fault caused by a faulty plugin:");
    yError("%s\n", stack_strings[2]);
    yarp_print_trace(stderr, __FILE__, __LINE__);

    // Free memory allocated by backtrace_symbols()
    free(stack_strings);

    // Restore original action
    sigaction(SIGSEGV, &old_action, nullptr);
}
#endif

RobotInterface::Module::Module() :
    mPriv(new Private(this))
{
#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__)
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    memset(&Private::old_action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = Private::sigsegv_handler;
    sigaction(SIGSEGV, &action, &Private::old_action);
#endif
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

    const std::string &filename = rf.findFile("config");
    yTrace() << "Reading robot config file" << filename;

    bool verbosity = rf.check("verbose");
    bool deprecated = rf.check("allow-deprecated-dtd");
    RobotInterface::XMLReader reader;
    reader.setVerbose(verbosity);
    reader.setEnableDeprecated(deprecated);
    mPriv->robot = reader.getRobot(filename);
    // yDebug() << mPriv->robot;

    // User can use YARP_PORT_PREFIX environment variable to override
    // the default name, so we don't care of handling the --name
    // argument
    setName(mPriv->robot.portprefix().c_str());

    mPriv->robot.setVerbose(verbosity);
    mPriv->robot.setAllowDeprecatedDevices(rf.check("allow-deprecated-devices"));

    std::string rpcPortName("/" + getName() + "/yarprobotinterface");
    mPriv->rpcPort.open(rpcPortName);
    attach(mPriv->rpcPort);

    // Enter startup phase
    if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseStartup) ||
        !mPriv->robot.enterPhase(RobotInterface::ActionPhaseRun)) {
        yError() << "Error in" << ActionPhaseToString(mPriv->robot.currentPhase()) << "phase... see previous messages for more info";
        // stopModule() calls interruptModule() internally.
        // This ensure that interrupt1 phase actions (i.e. detach) are
        // performed before destroying the devices when we call close();
        stopModule();
        // According to robotology/yarp#482, close() is not called by
        // runModule(rf), and the user is supposed to leave everything
        // clean.
        close();
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
    yDebug() << "yarprobotinterface running happily";
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
    if (mPriv->closed) {
        return mPriv->closeOk;
    }
    mPriv->closed = true;

    // If called from the first interrupt, enter the corresponding
    // interrupt phase.
    switch (mPriv->interruptReceived) {
    case 1:
        if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseInterrupt1)) {
            yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseInterrupt1) << "phase... see previous messages for more info";
            mPriv->closeOk = false;
        }
        break;
    case 2:
    case 3:
        break;
    default:
        mPriv->closeOk = false;
    }

    // Finally call the shutdown phase.
    if (!mPriv->robot.enterPhase(RobotInterface::ActionPhaseShutdown)) {
        yError() << "Error in" << ActionPhaseToString(RobotInterface::ActionPhaseShutdown) << "phase... see previous messages for more info";
        mPriv->closeOk = false;
    }

    mPriv->rpcPort.interrupt();
    mPriv->rpcPort.close();
    return mPriv->closeOk;
}

bool RobotInterface::Module::attach(yarp::os::RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}


std::string RobotInterface::Module::get_phase()
{
    return ActionPhaseToString(mPriv->robot.currentPhase());
}

int32_t RobotInterface::Module::get_level()
{
    return mPriv->robot.currentLevel();
}

bool RobotInterface::Module::is_ready()
{
    return (mPriv->robot.currentPhase() == RobotInterface::ActionPhaseRun ? true : false);
}

std::string RobotInterface::Module::get_robot()
{
    return mPriv->robot.name();
}

std::string RobotInterface::Module::quit()
{
    stopModule();
    return "bye";
}
