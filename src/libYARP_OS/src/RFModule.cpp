// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale, Anne van Rossum, Paul Fitzpatrick
* Copyright (C) 2009 The RobotCub consortium
* Based on code by Paul Fitzpatrick 2007.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Vocab.h>

//#include <ace/OS.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformTime.h>

using namespace yarp::os;
using namespace yarp::os::impl;



//time helper functions

void yarp::os::impl::getTime(ACE_Time_Value& now) {
#ifdef YARP_HAS_ACE
#  ifdef ACE_WIN32
    // now = ACE_High_Res_Timer::gettimeofday_hr();
    // Fixing, caused problems with new ACE versions and/or Windows 7.
    now = ACE_OS::gettimeofday();
#  else
    now = ACE_OS::gettimeofday ();
#  endif
#else
    struct timezone *tz = NULL;
    gettimeofday(&now, tz);
#endif
}

void yarp::os::impl::sleepThread(ACE_Time_Value& sleep_period)
{
#ifdef YARP_HAS_ACE
    if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
        sleep_period.set(0,0);
    ACE_OS::sleep(sleep_period);
#else
    if (sleep_period.tv_usec < 0 || sleep_period.tv_sec < 0) {
        sleep_period.tv_usec = 0;
        sleep_period.tv_sec = 0;
    }
    usleep(sleep_period.tv_sec * 1000000 + sleep_period.tv_usec );
#endif
}

void yarp::os::impl::addTime(ACE_Time_Value& val, const ACE_Time_Value & add) {
#ifdef YARP_HAS_ACE
    val += add;
#else
    val.tv_usec += add.tv_usec;
    int over = val.tv_usec % 1000000;
    if (over != val.tv_usec) {
        val.tv_usec = over;
        val.tv_sec++;
    }
    val.tv_sec += add.tv_sec;
#endif
}

void yarp::os::impl::subtractTime(ACE_Time_Value & val, const ACE_Time_Value & subtract) {
#ifdef YARP_HAS_ACE
    val -= subtract;
#else
    if (val.tv_usec > subtract.tv_usec) {
        val.tv_usec -= subtract.tv_usec;
        val.tv_sec -= subtract.tv_sec;
        return;
    }
    int over = 1000000 + val.tv_usec - subtract.tv_usec;
    val.tv_usec = over;
    val.tv_sec--;
    val.tv_sec -= subtract.tv_sec;
#endif
}

double yarp::os::impl::toDouble(const ACE_Time_Value &v) {
#ifdef YARP_HAS_ACE
    return double(v.sec()) + v.usec() * 1e-6;
#else
    return double(v.tv_sec) + v.tv_usec * 1e-6;
#endif
}

void yarp::os::impl::fromDouble(ACE_Time_Value &v, double x,int unit) {
#ifdef YARP_HAS_ACE
        v.msec(static_cast<int>(x*1000/unit+0.5));
#else
        v.tv_usec = static_cast<int>(x*1000000/unit+0.5) % 1000000;
        v.tv_sec = static_cast<int>(x/unit);
#endif
}


class RFModuleHelper : public yarp::os::PortReader, public Thread
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
    virtual bool read(yarp::os::ConnectionReader& connection);

    RFModuleHelper(RFModule& owner) : owner(owner), attachedToPort(false), attachedTerminal(false) {}

    virtual void run() {
        printf("Listening to terminal (type \"quit\" to stop module)\n");
        bool isEof = false;
        while (!(isEof||isStopping()||owner.isStopping())) {
            ConstString str = NetworkBase::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str.c_str());
                Bottle reply;
                bool ok = owner.safeRespond(cmd,reply);
                if (ok) {
                    //printf("ALL: %s\n", reply.toString().c_str());
                    //printf("ITEM 1: %s\n", reply.get(0).toString().c_str());
                    if (reply.get(0).toString()=="help") {
                        for (int i=0; i<reply.size(); i++) {
                            ACE_OS::printf("%s\n",
                                           reply.get(i).toString().c_str());
                        }
                    } else {
                        ACE_OS::printf("%s\n", reply.toString().c_str());
                    }
                } else {
                    ACE_OS::printf("Command not understood -- %s\n", str.c_str());
                }
            }
        }
        //printf("terminal shutting down\n");
        //owner.interruptModule();
    }

    bool attach(yarp::os::Port& source)
    {
        attachedToPort=true;
        source.setReader(*this);
        return true;
    }

    bool attach(yarp::os::RpcServer& source)
    {
        attachedToPort=true;
        source.setReader(*this);
        return true;
    }

    bool attachTerminal()
    {
        attachedTerminal=true;

        Thread::start();
        return true;
    }

    bool isTerminalAttached()
    {
        return attachedTerminal;
    }

    bool detachTerminal()
    {
        fprintf(stderr, "Critial: stopping thread, this might hang\n");
        Thread::stop();
        fprintf(stderr, "done!\n");
        return true;
    }

private:
public:

};

bool RFModuleHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    printf("command received: %s\n", cmd.toString().c_str());

    bool result = owner.safeRespond(cmd,response);
    if (response.size()>=1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=0) {
            if (response.get(0).toString()=="many" && writer->isTextMode()) {
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


#define HELPER(x) (*((RFModuleHelper*)(x)))

static RFModule *module = 0;
static void handler (int sig) {
    static int ct = 0;
    ct++;
    if (ct>3) {
        ACE_OS::printf("Aborting (calling exit())...\n");
        yarp::os::exit(1);
    }
    ACE_OS::printf("[try %d of 3] Trying to shut down\n",
                   ct);

    if (module!=0)
    {
        module->stopModule(false);
    }

  //  if (module!=NULL) {
  //      Bottle cmd, reply;
  //      cmd.fromString("quit");
   //     module->safeRespond(cmd,reply);
        //printf("sent %s, got %s\n", cmd.toString().c_str(),
        //     reply.toString().c_str());
   // }
}

// Special case for windows. Do not return, wait for the the main thread to return.
// In any case windows will shut down the application after a timeout of 5 seconds.
// This wait is required otherwise windows shuts down the process after we return from
// the signal handler. We could not find better way to handle clean remote shutdown of
// processes in windows.
#if defined(WIN32)
static void handler_sigbreak(int sig)
{
    raise(SIGINT);
}
#endif

RFModule::RFModule() {
    implementation = new RFModuleHelper(*this);
    stopFlag=false;

    //set up signal handlers for catching ctrl-c
    if (module==NULL) {
        module = this;
    }
    else {
        ACE_OS::printf("Module::Module() signal handling currently only good for one module\n");
    }

#if defined(WIN32)
    ACE_OS::signal(SIGBREAK, (ACE_SignalHandler) handler_sigbreak);
#endif

    ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) handler);
}

RFModule::~RFModule() {
    if (implementation!=0) {
        //HELPER(implementation).stop();
        delete &HELPER(implementation);
        implementation = 0;
    }
}

 /**
* Attach this object to a source of messages.
* @param source a BufferedPort or PortReaderBuffer that
* receives data.
*/
bool RFModule::attach(yarp::os::Port &source) {
    HELPER(implementation).attach(source);
    return true;
}

bool RFModule::attach(yarp::os::RpcServer &source) {
    HELPER(implementation).attach(source);
    return true;
}

bool RFModule::basicRespond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab()) {
    case VOCAB4('q','u','i','t'):
    case VOCAB4('e','x','i','t'):
    case VOCAB3('b','y','e'):
        reply.addVocab(Vocab::encode("bye"));
        stopModule(false); //calls interruptModule()
   //     interruptModule();
        return true;
    default:
        reply.add("command not recognized");
        return false;
    }
    return false;
}

bool RFModule::safeRespond(const Bottle& command, Bottle& reply) {
    bool ok = respond(command,reply);
    if (!ok) {
        // just in case derived classes don't correctly pass on messages
        ok = basicRespond(command,reply);
    }
    return ok;
}

int RFModule::runModule() {
    //setting up main loop

    ACE_Time_Value currentRunTV;
    ACE_Time_Value elapsedTV;
    ACE_Time_Value sleepPeriodTV;
    ACE_Time_Value oneSecTV;

    fromDouble(oneSecTV, 1.0);

    while (!isStopping()) {
        getTime(currentRunTV);

        // If updateModule() returns false we exit the main loop.
        if(!updateModule()) {
            break;
        }

        // The module is stopped for getPeriod() seconds.
        // If getPeriod() returns a time > 1 second, we check every second if
        // the module stopping, and eventually we exit the main loop.
        do {
            getTime(elapsedTV);
            fromDouble(sleepPeriodTV,getPeriod());
            addTime(sleepPeriodTV, currentRunTV);
            subtractTime(sleepPeriodTV, elapsedTV);
            if (sleepPeriodTV.msec() > 1000) {
                sleepThread(oneSecTV);
            } else {
                sleepThread(sleepPeriodTV);
                break;
            }
        } while (!isStopping());
        getTime(currentRunTV);
    }

    ACE_OS::printf("Module closing\n");
    if (HELPER(implementation).isTerminalAttached())
    {
        ACE_OS::fprintf(stderr, "WARNING: module attached to terminal calling exit() to quit.\n");
        ACE_OS::fprintf(stderr, "You should be aware that this is not a good way to stop a module. Effects will be:\n");
       // ACE_OS::fprintf(stderr, "- the module close() function will NOT be called\n");
        ACE_OS::fprintf(stderr, "- class destructors will NOT be called\n");
        ACE_OS::fprintf(stderr, "- code in the main after runModule() will NOT be executed\n");
        ACE_OS::fprintf(stderr, "This happens because in your module you called attachTerminal() and we don't have a portable way to quit");
        ACE_OS::fprintf(stderr, " a module that is listening to the terminal.\n");
        ACE_OS::fprintf(stderr, "At the moment the only way to have the module quit correctly is to avoid listening to terminal");
        ACE_OS::fprintf(stderr, "(i.e. do not call attachTerminal()).\n");
        ACE_OS::fprintf(stderr, "This will also make this annoying message go away.\n");


         //one day this will hopefully go away, now only way to stop
        // remove both:
        close();
        ACE_OS::exit(1);
        /////////////////////////////////////////////////////////////
        detachTerminal();
    }

    close();
    ACE_OS::printf("Module finished\n");
    return 0;
}


int RFModule::runModule(yarp::os::ResourceFinder &rf) {
    if (!configure(rf)) {
        ACE_OS::printf("Module failed to open\n");
        return false;
    }
    return runModule();
}

ConstString RFModule::getName(const char *subName) {
    if (subName==0) {
        return name;
    }

    String base = name.c_str();

    // Support legacy behavior, check if a "/" needs to be
    // appended before subName.
    if (subName[0]!='/')
    {
        ACE_OS::printf("WARNING: subName in getName() does not begin with \"/\" this suggest you expect getName() to follow a deprecated behavior.\n");
        ACE_OS::printf("I am now adding \"/\" between %s and %s but you should not rely on this.\n", name.c_str(), subName);

        base += "/";
    }

    base += subName;
    return base.c_str();
}

bool RFModule::attachTerminal() {
    HELPER(implementation).attachTerminal();
    return true;
}

bool RFModule::detachTerminal()
{
    HELPER(implementation).detachTerminal();
    return true;
}

