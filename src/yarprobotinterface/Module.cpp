/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Module.h"

#include <yarp/robotinterface/Action.h>
#include <yarp/robotinterface/Device.h>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/Robot.h>
#include <yarp/robotinterface/XMLReader.h>

#include <yarp/conf/system.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcServer.h>

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__) && !defined(__arm__) && !defined(__aarch64__)
#  include <csignal>
#  include <cstring>
#  include <execinfo.h>
#endif

class yarprobotinterface::Module::Private
{
public:
    Private(Module *parent);
    ~Private();

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__) && !defined(__arm__) && !defined(__aarch64__)
    static struct sigaction old_action;
    static void sigsegv_handler(int nSignum, siginfo_t* si, void* vcontext);
#endif

    Module * const parent;
    yarp::robotinterface::Robot robot;
    int interruptReceived;
    yarp::os::RpcServer rpcPort;
    bool closed;
    bool closeOk;
};

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__) && !defined(__arm__) && !defined(__aarch64__)
struct sigaction yarprobotinterface::Module::Private::old_action;
#endif

yarprobotinterface::Module::Private::Private(Module *parent) :
    parent(parent),
    interruptReceived(0),
    closed(false),
    closeOk(true)
{
}

yarprobotinterface::Module::Private::~Private() = default;

#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__) && !defined(__arm__) && !defined(__aarch64__)
void yarprobotinterface::Module::Private::sigsegv_handler(int nSignum, siginfo_t* si, void* vcontext)
{
    auto context = reinterpret_cast<ucontext_t*>(vcontext);
#if defined(__x86_64__)
    context->uc_mcontext.gregs[REG_RIP]++;
#else
    context->uc_mcontext.gregs[REG_EIP]++;
#endif

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

yarprobotinterface::Module::Module() :
    mPriv(new Private(this))
{
#if defined(YARP_HAS_EXECINFO_H) && !defined(__APPLE__) && !defined(__arm__) && !defined(__aarch64__)
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    memset(&Private::old_action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = Private::sigsegv_handler;
    sigaction(SIGSEGV, &action, &Private::old_action);
#endif
}

yarprobotinterface::Module::~Module()
{
    delete mPriv;
}

bool yarprobotinterface::Module::configure(yarp::os::ResourceFinder& rf)
{
    if (!rf.check("config")) {
        yFatal() << "Missing \"config\" argument";
    }

    const std::string& filename = rf.findFile("config");
    yTrace() << "Reading robot config file" << filename;

    bool verbosity = rf.check("verbose");
    bool deprecated = rf.check("allow-deprecated-dtd");
    yarp::robotinterface::XMLReader reader;
    reader.setVerbose(verbosity);
    reader.setEnableDeprecated(deprecated);

    // Prepare configuration for sub-devices
    yarp::os::Property config;
    config.fromString(rf.toString());
    // The --config option is consumed by yarprobotinterface, and never
    // forwarded to the devices)
    config.unput("config");

    yarp::robotinterface::XMLReaderResult result = reader.getRobotFromFile(filename, config);

    if (!result.parsingIsSuccessful) {
        yFatal() << "Config file " << filename << " not parsed correctly.";
    }

    mPriv->robot = std::move(result.robot);
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
    if (!mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseStartup) ||
        !mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseRun)) {
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

double yarprobotinterface::Module::getPeriod()
{
    return 60;
}

bool yarprobotinterface::Module::updateModule()
{
    yDebug() << "yarprobotinterface running happily";
    return true;
}

bool yarprobotinterface::Module::interruptModule()
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
        if (!mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt2)) {
            yError() << "Error in" << ActionPhaseToString(yarp::robotinterface::ActionPhaseInterrupt2) << "phase... see previous messages for more info";
            return false;
        }
        break;
    case 3:
        if (!mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt3)) {
            yError() << "Error in" << ActionPhaseToString(yarp::robotinterface::ActionPhaseInterrupt3) << "phase... see previous messages for more info";
            return false;
        }
        break;
    default:
        return false;
    }

    return true;
}

bool yarprobotinterface::Module::close()
{
    if (mPriv->closed) {
        return mPriv->closeOk;
    }
    mPriv->closed = true;

    // If called from the first interrupt, enter the corresponding
    // interrupt phase.
    switch (mPriv->interruptReceived) {
    case 1:
        if (!mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1)) {
            yError() << "Error in" << ActionPhaseToString(yarp::robotinterface::ActionPhaseInterrupt1) << "phase... see previous messages for more info";
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
    if (!mPriv->robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown)) {
        yError() << "Error in" << ActionPhaseToString(yarp::robotinterface::ActionPhaseShutdown) << "phase... see previous messages for more info";
        mPriv->closeOk = false;
    }

    mPriv->rpcPort.interrupt();
    mPriv->rpcPort.close();
    return mPriv->closeOk;
}

bool yarprobotinterface::Module::attach(yarp::os::RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}


std::string yarprobotinterface::Module::get_phase()
{
    return ActionPhaseToString(mPriv->robot.currentPhase());
}

int32_t yarprobotinterface::Module::get_level()
{
    return mPriv->robot.currentLevel();
}

bool yarprobotinterface::Module::is_ready()
{
    return (mPriv->robot.currentPhase() == yarp::robotinterface::ActionPhaseRun ? true : false);
}

std::string yarprobotinterface::Module::get_robot()
{
    return mPriv->robot.name();
}

std::string yarprobotinterface::Module::quit()
{
    stopModule();
    return "bye";
}
