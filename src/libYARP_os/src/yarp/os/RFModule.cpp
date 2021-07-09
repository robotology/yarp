/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/RFModule.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/Terminal.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(RFMODULE, "yarp.os.RFModule")
}

class RFModuleRespondHandler :
        public yarp::os::PortReader,
        public yarp::os::Thread
{
private:
    RFModule& owner;
    bool attachedToPort;
    bool attachedTerminal;

public:
    /**
     * Handler for reading messages from the network, and passing
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    bool read(yarp::os::ConnectionReader& connection) override;


    RFModuleRespondHandler(RFModule& owner) :
            owner(owner),
            attachedToPort(false),
            attachedTerminal(false)
    {
    }


    void run() override
    {
        yCInfo(RFMODULE, "Listening to terminal (type \"quit\" to stop module).");
        bool isEof = false;
        while (!(isEof || isStopping() || owner.isStopping())) {
            std::string str = yarp::os::impl::Terminal::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str);
                Bottle reply;
                bool ok = owner.safeRespond(cmd, reply);
                if (ok) {
                    yCDebug(RFMODULE, "ALL: %s\n", reply.toString().c_str());
                    yCDebug(RFMODULE, "ITEM 1: %s\n", reply.get(0).toString().c_str());
                    if (reply.get(0).toString() == "help") {
                        for (size_t i = 0; i < reply.size(); i++) {
                            yCInfo(RFMODULE, "%s.", reply.get(i).toString().c_str());
                        }
                    } else {
                        yCInfo(RFMODULE, "%s.", reply.toString().c_str());
                    }
                } else {
                    yCInfo(RFMODULE, "Command not understood -- %s.", str.c_str());
                }
            }
        }
        yCDebug(RFMODULE, "terminal shutting down\n");
        //owner.interruptModule();
    }


    bool attach(yarp::os::Port& source)
    {
        attachedToPort = true;
        source.setReader(*this);
        return true;
    }


    bool attach(yarp::os::RpcServer& source)
    {
        attachedToPort = true;
        source.setReader(*this);
        return true;
    }


    bool attachTerminal()
    {
        attachedTerminal = true;

        Thread::start();
        return true;
    }


    bool isTerminalAttached()
    {
        return attachedTerminal;
    }


    bool detachTerminal()
    {
        yCWarning(RFMODULE, "Critical: stopping thread, this might hang.");
        Thread::stop();
        yCWarning(RFMODULE, "done!");
        return true;
    }
};

bool RFModuleRespondHandler::read(ConnectionReader& connection)
{
    Bottle cmd;
    Bottle response;
    if (!cmd.read(connection)) {
        return false;
    }
    yCDebug(RFMODULE, "command received: %s\n", cmd.toString().c_str());

    bool result = owner.safeRespond(cmd, response);
    if (response.size() >= 1) {
        ConnectionWriter* writer = connection.getWriter();
        if (writer != nullptr) {
            if (response.get(0).toString() == "many" && writer->isTextMode()) {
                for (size_t i = 1; i < response.size(); i++) {
                    Value& v = response.get(i);
                    if (v.isList()) {
                        v.asList()->write(*writer);
                    } else {
                        Bottle b;
                        b.add(v);
                        b.write(*writer);
                    }
                }
            } else {
                response.write(*writer);
            }
            yCDebug(RFMODULE, "response sent: %s\n", response.toString().c_str());
        }
    }
    return result;
}


class RFModuleThreadedHandler : public Thread
{
private:
    RFModule& owner;

public:
    RFModuleThreadedHandler(RFModule& owner) :
            owner(owner)
    {
    }

    void run() override
    {
        owner.runModule();
    }
};


class RFModule::Private
{
private:
    RFModule& owner;
    bool singleton_run_module;

public:
    RFModuleRespondHandler* respond_handler;
    RFModuleThreadedHandler* threaded_handler;

    Private(RFModule& owner) :
            owner(owner),
            singleton_run_module(false),
            respond_handler(nullptr),
            threaded_handler(nullptr)
    {
        respond_handler = new RFModuleRespondHandler(owner);
    }

    ~Private()
    {
        delete respond_handler;
        delete threaded_handler;
    }

    bool newThreadHandler()
    {
        threaded_handler = new RFModuleThreadedHandler(owner);

        return threaded_handler != nullptr;
    }

    void deleteThreadHandler()
    {
        delete threaded_handler;
        threaded_handler = nullptr;
    }


    bool getSingletonRunModule()
    {
        return singleton_run_module;
    }
    void setSingletonRunModule()
    {
        singleton_run_module = true;
    }
};


static RFModule* module = nullptr;

int RFModule::getThreadKey()
{
    if (mPriv->threaded_handler != nullptr) {
        return mPriv->threaded_handler->getKey();
    }
    return yarp::os::Thread::getKeyOfCaller();
}

static void handler(int sig)
{
    YARP_UNUSED(sig);
    static int ct = 0;
    ct++;
    yarp::os::Time::useSystemClock();
    if (ct > 3) {
        yCInfo(RFMODULE, "Aborting (calling abort())...");
        std::abort();
    }
    yCInfo(RFMODULE, "[try %d of 3] Trying to shut down.", ct);

    if (module != nullptr) {
        module->stopModule(false);
    }

#if defined(_WIN32)
    // on windows we need to reset the handler after being called, otherwise it
    // will not be called anymore.
    // see http://www.geeksforgeeks.org/write-a-c-program-that-doesnt-terminate-when-ctrlc-is-pressed/

    // Additionally, from
    // http://www.linuxprogrammingblog.com/all-about-linux-signals?page=show
    // The signal(2) function is the oldest and simplest way to install a signal
    // handler but it's deprecated.
    // There are few reasons and most important is that the original Unix
    // implementation would reset the signal handler to it's default value after
    // signal is received.
    ::signal(SIGINT, handler);
#endif
}

// Special case for windows. Do not return, wait for the the main thread to
// return.  In any case windows will shut down the application after a timeout
// of 5 seconds.  This wait is required otherwise windows shuts down the process
// after we return from the signal handler.  We could not find better way to
// handle clean remote shutdown of processes in windows.
#if defined(_WIN32)
static void handler_sigbreak(int sig)
{
    yarp::os::impl::raise(SIGINT);
}
#endif


RFModule::RFModule() :
        stopFlag(false),
        mPriv(nullptr)
{
    yarp::os::Network::initMinimum();

    mPriv = new Private(*this);

    //set up signal handlers for catching ctrl-c
    if (module == nullptr) {
        module = this;
    } else {
        yCInfo(RFMODULE, "RFModule::RFModule() signal handling currently only good for one module.");
    }

#if defined(_WIN32)
    yarp::os::impl::signal(SIGBREAK, handler_sigbreak);
#endif

    yarp::os::impl::signal(SIGINT, handler);
    yarp::os::impl::signal(SIGTERM, handler);
}


RFModule::~RFModule()
{
    delete mPriv;
    yarp::os::Network::finiMinimum();
}


double RFModule::getPeriod()
{
    return 1.0;
}


int RFModule::runModule()
{
    if (mPriv->getSingletonRunModule()) {
        return 1;
    }
    mPriv->setSingletonRunModule();

    // Setting up main loop
    // Use yarp::os::Time
    double currentRun;
    double elapsed;
    double sleepPeriod;

    while (!isStopping()) {
        currentRun = yarp::os::Time::now();
        // If updateModule() returns false we exit the main loop.
        if (!updateModule()) {
            break;
        }

        // At the end of each run of updateModule function, the thread is supposed
        // to be suspended and release CPU to other threads.
        // Calling a yield here will help the threads to alternate in the execution.
        // Note: call yield BEFORE computing elapsed time, so that any time spent due to
        // yield is took into account and the sleep time is correct.
        yarp::os::Time::yield();

        // The module is stopped for getPeriod() seconds.
        // If getPeriod() returns a time > 1 second, we check every second if
        // the module stopping, and eventually we exit the main loop.
        do {
            elapsed = yarp::os::Time::now() - currentRun;
            sleepPeriod = getPeriod() - elapsed;
            if (sleepPeriod > 1) {
                Time::delay(1.0);
            } else {
                Time::delay(sleepPeriod);
                break;
            }
        } while (!isStopping());
    }

    yCInfo(RFMODULE, "RFModule closing.");
    if (mPriv->respond_handler->isTerminalAttached()) {
        yCWarning(RFMODULE, "Module attached to terminal calling exit() to quit.");
        yCWarning(RFMODULE, "You should be aware that this is not a good way to stop a module. Effects will be:");
        yCWarning(RFMODULE, "- class destructors will NOT be called");
        yCWarning(RFMODULE, "- code in the main after runModule() will NOT be executed");
        yCWarning(RFMODULE, "This happens because in your module you called attachTerminal() and we don't have a portable way to quit a module that is listening to the terminal.");
        yCWarning(RFMODULE, "At the moment the only way to have the module quit correctly is to avoid listening to terminal (i.e. do not call attachTerminal()).");
        yCWarning(RFMODULE, "This will also make this annoying message go away.");

        // One day this will hopefully go away, now only way to stop
        // remove both:
        close();
        std::exit(1);
        /////////////////////////////////////////////////////////////
        detachTerminal();
    }

    close();
    yCInfo(RFMODULE, "RFModule finished.");
    return 0;
}


int RFModule::runModule(yarp::os::ResourceFinder& rf)
{
    if (mPriv->getSingletonRunModule()) {
        return 1;
    }

    if (!configure(rf)) {
        yCInfo(RFMODULE, "RFModule failed to open.");
        return 1;
    }
    return runModule();
}


int RFModule::runModuleThreaded()
{
    if (mPriv->getSingletonRunModule()) {
        return 1;
    }

    if (!mPriv->newThreadHandler()) {
        yCError(RFMODULE, "RFModule handling thread failed to allocate.");
        return 1;
    }

    if (!mPriv->threaded_handler->start()) {
        yCError(RFMODULE, "RFModule handling thread failed to start.");
        return 1;
    }

    return 0;
}


int RFModule::runModuleThreaded(ResourceFinder& rf)
{
    if (mPriv->getSingletonRunModule()) {
        return 1;
    }

    if (!configure(rf)) {
        yCError(RFMODULE, "RFModule failed to open.");
        return 1;
    }

    return runModuleThreaded();
}


bool RFModule::configure(yarp::os::ResourceFinder& rf)
{
    YARP_UNUSED(rf);
    return true;
}


bool RFModule::respond(const Bottle& command, Bottle& reply)
{
    return basicRespond(command, reply);
}


/**
* Attach this object to a source of messages.
* @param source a BufferedPort or PortReaderBuffer that
* receives data.
*/
bool RFModule::attach(yarp::os::Port& source)
{
    mPriv->respond_handler->attach(source);
    return true;
}


bool RFModule::attach(yarp::os::RpcServer& source)
{
    mPriv->respond_handler->attach(source);
    return true;
}


bool RFModule::attachTerminal()
{
    mPriv->respond_handler->attachTerminal();
    return true;
}


bool RFModule::detachTerminal()
{
    mPriv->respond_handler->detachTerminal();
    return true;
}


bool RFModule::interruptModule()
{
    return true;
}


bool RFModule::close()
{
    return true;
}


void RFModule::stopModule(bool wait)
{
    stopFlag = true;

    if (!interruptModule()) {
        yCError(RFMODULE, "interruptModule() returned an error there could be problems shutting down the module.");
    }

    if (wait) {
        joinModule();
    }
}


bool RFModule::isStopping()
{
    return stopFlag;
}


bool RFModule::joinModule(double seconds)
{
    if (mPriv->threaded_handler != nullptr) {
        if (mPriv->threaded_handler->join(seconds)) {
            mPriv->deleteThreadHandler();
            return true;
        }
        yCWarning(RFMODULE, "RFModule joinModule() timed out.");
        return false;
    }
    yCWarning(RFMODULE, "Cannot call join: RFModule runModule() is not currently threaded.");
    return true;
}


std::string RFModule::getName(const std::string& subName)
{
    if (subName.empty()) {
        return name;
    }

    std::string base = name;

    // Support legacy behavior, check if a "/" needs to be
    // appended before subName.
    if (subName[0] != '/') {
        yCWarning(RFMODULE, "SubName in getName() does not begin with \"/\" this suggest you expect getName() to follow a deprecated behavior.");
        yCWarning(RFMODULE, "I am now adding \"/\" between %s and %s but you should not rely on this.", name.c_str(), subName.c_str());

        base += "/";
    }

    base += subName;
    return base;
}


void RFModule::setName(const char* name)
{
    this->name = name;
}


bool RFModule::safeRespond(const Bottle& command, Bottle& reply)
{
    bool ok = respond(command, reply);
    if (!ok) {
        // just in case derived classes don't correctly pass on messages
        ok = basicRespond(command, reply);
    }
    return ok;
}


bool RFModule::basicRespond(const Bottle& command, Bottle& reply)
{
    switch (command.get(0).asVocab32()) {
    case yarp::os::createVocab32('q', 'u', 'i', 't'):
    case yarp::os::createVocab32('e', 'x', 'i', 't'):
    case yarp::os::createVocab32('b', 'y', 'e'):
        reply.addVocab32("bye");
        stopModule(false); //calls interruptModule()
        return true;
    default:
        reply.addString("command not recognized");
        return false;
    }
    return false;
}
