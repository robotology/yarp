/*
* Copyright (C) 2009 The RobotCub consortium
* Author: Lorenzo Natale, Anne van Rossum, Paul Fitzpatrick
* Based on code by Paul Fitzpatrick 2007.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/Bottle.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/impl/PlatformSignal.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::os::impl;


class RFModuleRespondHandler : public yarp::os::PortReader, public Thread {
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
    virtual bool read(yarp::os::ConnectionReader& connection) override;


    RFModuleRespondHandler(RFModule& owner) : owner(owner), attachedToPort(false), attachedTerminal(false) {}


    virtual void run() override {
        yInfo("Listening to terminal (type \"quit\" to stop module).");
        bool isEof = false;
        while (!(isEof || isStopping() || owner.isStopping())) {
            ConstString str = NetworkBase::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str.c_str());
                Bottle reply;
                bool ok = owner.safeRespond(cmd, reply);
                if (ok) {
                    //printf("ALL: %s\n", reply.toString().c_str());
                    //printf("ITEM 1: %s\n", reply.get(0).toString().c_str());
                    if (reply.get(0).toString() == "help") {
                        for (int i = 0; i < reply.size(); i++) {
                            yInfo("%s.", reply.get(i).toString().c_str());
                        }
                    } else {
                        yInfo("%s.", reply.toString().c_str());
                    }
                } else {
                    yInfo("Command not understood -- %s.", str.c_str());
                }
            }
        }
        //printf("terminal shutting down\n");
        //owner.interruptModule();
    }


    bool attach(yarp::os::Port& source) {
        attachedToPort=true;
        source.setReader(*this);
        return true;
    }


    bool attach(yarp::os::RpcServer& source) {
        attachedToPort=true;
        source.setReader(*this);
        return true;
    }


    bool attachTerminal() {
        attachedTerminal=true;

        Thread::start();
        return true;
    }


    bool isTerminalAttached() {
        return attachedTerminal;
    }


    bool detachTerminal() {
        yWarning("Critial: stopping thread, this might hang.");
        Thread::stop();
        yWarning("done!");
        return true;
    }
};


bool RFModuleRespondHandler::read(ConnectionReader& connection) {
    Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    //printf("command received: %s\n", cmd.toString().c_str());

    bool result = owner.safeRespond(cmd, response);
    if (response.size() >= 1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=YARP_NULLPTR) {
            if (response.get(0).toString() == "many" && writer->isTextMode()) {
                for (int i=1; i<response.size(); i++) {
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
            //printf("response sent: %s\n", response.toString().c_str());
        }
    }
    return result;
}


class RFModuleThreadedHandler : public Thread {
private:
    RFModule& owner;

public:
    RFModuleThreadedHandler(RFModule& owner) : owner(owner) {};

    void run() override { owner.runModule(); }
};


class RFModuleHelper {
private:
    RFModule& owner;
    bool      singleton_run_module;

public:
    RFModuleRespondHandler  *respond_handler;
    RFModuleThreadedHandler *threaded_handler;


    RFModuleHelper(RFModule& owner) : owner(owner), singleton_run_module(false), respond_handler(YARP_NULLPTR), threaded_handler(YARP_NULLPTR) {
        respond_handler  = new RFModuleRespondHandler(owner);
    }

    ~RFModuleHelper() {
        if (respond_handler != YARP_NULLPTR) {
            delete respond_handler;
            respond_handler = YARP_NULLPTR;
        }
        if (threaded_handler != YARP_NULLPTR) {
            delete threaded_handler;
            threaded_handler = YARP_NULLPTR;
        }
    }


    bool newThreadHandler() {
        threaded_handler = new RFModuleThreadedHandler(owner);

        if (threaded_handler != YARP_NULLPTR) return true;
        else                                  return false;
    }

    void deleteThreadHandler() {
        delete threaded_handler;
        threaded_handler = YARP_NULLPTR;
    }


    bool getSingletonRunModule() { return singleton_run_module; }
    void setSingletonRunModule() { singleton_run_module = true; }
};


#define HELPER(x)           (*((RFModuleHelper*)(x)))
#define RESPOND_HANDLER(x)  (*(HELPER(x).respond_handler))
#define THREADED_HANDLER(x) (*(HELPER(x).threaded_handler))


static RFModule *module = YARP_NULLPTR;


static void handler (int sig) {
    static int ct = 0;
    ct++;
    yarp::os::Time::useSystemClock();
    if (ct > 3) {
        yInfo("Aborting (calling abort())...");
        std::abort();
    }
    yInfo("[try %d of 3] Trying to shut down.", ct);

    if (module != YARP_NULLPTR) {
        module->stopModule(false);
    }

//    if (module!=YARP_NULLPTR) {
//        Bottle cmd, reply;
//        cmd.fromString("quit");
//        module->safeRespond(cmd, reply);
//        printf("sent %s, got %s\n", cmd.toString().c_str(),
//             reply.toString().c_str());
//    }

#if defined(_WIN32)
    // on windows we need to reset the handler after beeing called, otherwise it
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
static void handler_sigbreak(int sig) {
    yarp::os::impl::raise(SIGINT);
}
#endif


RFModule::RFModule() {
    yarp::os::Network::initMinimum();
    implementation = new RFModuleHelper(*this);
    stopFlag = false;

    //set up signal handlers for catching ctrl-c
    if (module == YARP_NULLPTR) {
        module = this;
    }
    else {
        yInfo("RFModule::RFModule() signal handling currently only good for one module.");
    }

#if defined(_WIN32)
    yarp::os::impl::signal(SIGBREAK, handler_sigbreak);
#endif

    yarp::os::impl::signal(SIGINT, handler);
    yarp::os::impl::signal(SIGTERM, handler);
}


RFModule::~RFModule() {
    if (implementation != YARP_NULLPTR) {
        //HELPER(implementation).stop();
        delete &HELPER(implementation);
        implementation = YARP_NULLPTR;
    }
    yarp::os::Network::finiMinimum();
}


double RFModule::getPeriod() {
    return 1.0;
}


int RFModule::runModule() {
    if (HELPER(implementation).getSingletonRunModule()) return 1;
    HELPER(implementation).setSingletonRunModule();

    // Setting up main loop
    // Use yarp::os::Time
    double currentRun;
    double elapsed;
    double sleepPeriod;

    while (!isStopping())
    {
        currentRun = yarp::os::Time::now();
        // If updateModule() returns false we exit the main loop.
        if (!updateModule()) {
            break;
        }

        // The module is stopped for getPeriod() seconds.
        // If getPeriod() returns a time > 1 second, we check every second if
        // the module stopping, and eventually we exit the main loop.
        do
        {
            elapsed = yarp::os::Time::now() - currentRun;
            sleepPeriod = getPeriod() - elapsed;
            if(sleepPeriod > 1)
            {
                Time::delay(1.0);
            }
            else
            {
                Time::delay(sleepPeriod);
                break;
            }
        } while (!isStopping());
    }

    yInfo("RFModule closing.");
    if (RESPOND_HANDLER(implementation).isTerminalAttached())
    {
        yWarning("Module attached to terminal calling exit() to quit.");
        yWarning("You should be aware that this is not a good way to stop a module. Effects will be:");
//        yWarning("- the module close() function will NOT be called");
        yWarning("- class destructors will NOT be called");
        yWarning("- code in the main after runModule() will NOT be executed");
        yWarning("This happens because in your module you called attachTerminal() and we don't have a portable way to quit a module that is listening to the terminal.");
        yWarning("At the moment the only way to have the module quit correctly is to avoid listening to terminal (i.e. do not call attachTerminal()).");
        yWarning("This will also make this annoying message go away.");

        // One day this will hopefully go away, now only way to stop
        // remove both:
        close();
        std::exit(1);
        /////////////////////////////////////////////////////////////
        detachTerminal();
    }

    close();
    yInfo("RFModule finished.");
    return 0;
}


int RFModule::runModule(yarp::os::ResourceFinder &rf) {
    if (HELPER(implementation).getSingletonRunModule()) return 1;

    if (!configure(rf)) {
        yInfo("RFModule failed to open.");
        return 1;
    }
    return runModule();
}


int RFModule::runModuleThreaded() {
    if (HELPER(implementation).getSingletonRunModule()) return 1;

    if (!HELPER(implementation).newThreadHandler()) {
        yError("RFModule handling thread failed to allocate.");
        return 1;
    }

    if (!THREADED_HANDLER(implementation).start()) {
        yError("RFModule handling thread failed to start.");
        return 1;
    }

    return 0;
}


int RFModule::runModuleThreaded(ResourceFinder &rf) {
    if (HELPER(implementation).getSingletonRunModule()) return 1;

    if (!configure(rf)) {
        yError("RFModule failed to open.");
        return 1;
    }

    return runModuleThreaded();
}


bool RFModule::configure(yarp::os::ResourceFinder &rf) {
    return true;
}


bool RFModule::respond(const Bottle& command, Bottle& reply) {
    return basicRespond(command, reply);
}


/**
* Attach this object to a source of messages.
* @param source a BufferedPort or PortReaderBuffer that
* receives data.
*/
bool RFModule::attach(yarp::os::Port &source) {
    RESPOND_HANDLER(implementation).attach(source);
    return true;
}


bool RFModule::attach(yarp::os::RpcServer &source) {
    RESPOND_HANDLER(implementation).attach(source);
    return true;
}


bool RFModule::attachTerminal() {
    RESPOND_HANDLER(implementation).attachTerminal();
    return true;
}


bool RFModule::detachTerminal()
{
    RESPOND_HANDLER(implementation).detachTerminal();
    return true;
}


bool RFModule::interruptModule() {
    return true;
}


bool RFModule::close() {
    return true;
}


void RFModule::stopModule(bool wait) {
    stopFlag = true;

    if (!interruptModule()) {
        yError("interruptModule() returned an error there could be problems shutting down the module.");
    }

    if (wait) joinModule();
}


bool RFModule::isStopping() {
    return stopFlag;
}


bool RFModule::joinModule(double seconds) {
    if (&THREADED_HANDLER(implementation) != YARP_NULLPTR) {
        if (THREADED_HANDLER(implementation).join(seconds)) {
            HELPER(implementation).deleteThreadHandler();
            return true;
        } else {
            yWarning("RFModule joinModule() timed out.");
            return false;
        }
    } else {
        yWarning("Cannot call join: RFModule runModule() is not currently threaded.");
        return true;
    }
}


ConstString RFModule::getName(const ConstString& subName) {
    if (subName == "") {
        return name;
    }

    ConstString base = name.c_str();

    // Support legacy behavior, check if a "/" needs to be
    // appended before subName.
    if (subName[0] != '/') {
        yWarning("SubName in getName() does not begin with \"/\" this suggest you expect getName() to follow a deprecated behavior.");
        yWarning("I am now adding \"/\" between %s and %s but you should not rely on this.", name.c_str(), subName.c_str());

        base += "/";
    }

    base += subName;
    return base.c_str();
}


void RFModule::setName(const char *name) {
    this->name = name;
}


bool RFModule::safeRespond(const Bottle& command, Bottle& reply) {
    bool ok = respond(command, reply);
    if (!ok) {
        // just in case derived classes don't correctly pass on messages
        ok = basicRespond(command, reply);
    }
    return ok;
}


bool RFModule::basicRespond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab()) {
    case VOCAB4('q', 'u', 'i', 't'):
    case VOCAB4('e', 'x', 'i', 't'):
    case VOCAB3('b', 'y', 'e'):
        reply.addVocab(Vocab::encode("bye"));
        stopModule(false); //calls interruptModule()
   //     interruptModule();
        return true;
    default:
        reply.addString("command not recognized");
        return false;
    }
    return false;
}
