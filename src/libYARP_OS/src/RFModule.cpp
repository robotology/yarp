// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Lorenzo Natale.
* Copyright (C) 2009 The RobotCub consortium
* Based on code by Paul Fitzpatrick 2007.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Network.h>

//#include <ace/OS.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h> 
#include <ace/OS_NS_signal.h>

#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

using namespace yarp::os;
using namespace yarp::os::impl;



//time helper functions

inline ACE_Time_Value getTime()
{
    ACE_Time_Value now;
#ifdef ACE_WIN32
    now = ACE_High_Res_Timer::gettimeofday_hr();
#else
    now = ACE_OS::gettimeofday ();
#endif
    return now;
}

inline void sleepThread(ACE_Time_Value sleep_period)
{
    if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
        sleep_period.set(0,0);
    ACE_OS::sleep(sleep_period);
}

inline double toDouble(const ACE_Time_Value &v)
{
    return double(v.sec()) + v.usec() * 1e-6; 
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
            if (response.get(0).toString()=="many") {
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

RFModule::RFModule() {
    implementation = new RFModuleHelper(*this);
    stopFlag=false;
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


static RFModule *module = 0;
static void handler (int sig) {
    static int ct = 0;
    ct++;
    if (ct>3) {
        ACE_OS::printf("Aborting (calling exit())...\n");
        ACE_OS::exit(1);
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

	// Special case for windows. Do not return, wait for the the main thread to return.
    // In any case windows will shut down the application after a timeout of 5 seconds.
    // This wait is required otherwise windows shuts down the process after we return from
	// the signal handler. We could not find better way to handle clean remote shutdown of 
    // processes in windows.
    #if defined(WIN32) || defined(WIN64)
    if (sig==CTRL_CLOSE_EVENT) //other event types: CTRL_BREAK_EVENT CTRL_C_EVENT, CTRL_LOGOFF_EVENT, CTRL_SHUTDOWN_EVENT
    {
        yarp::os::Time::delay(20.0);
    }
    #endif
}


int RFModule::runModule() {
    stopFlag=false;

    //set up signal handlers for catching ctr-c
    if (module==NULL) {
        module = this;
    } 
    else {
        ACE_OS::printf("Module::runModule() signal handling currently only good for one module\n");
    }
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) handler);

#if defined(WIN32) || defined(WIN64)
    ACE_OS::signal(SIGBREAK, (ACE_SignalHandler) handler);
#endif

    //setting up main loop
    bool loop=true;

    ACE_Time_Value currentRunTV;
    ACE_Time_Value elapsedTV;
    ACE_Time_Value sleepPeriodTV;

    while (loop) {
        currentRunTV=getTime();
        loop=updateModule();
        elapsedTV=getTime();

        if (isStopping())
            loop=false;

        sleepPeriodTV.msec(static_cast<int>(getPeriod()*1000+0.5));
        sleepPeriodTV+=currentRunTV;
        sleepPeriodTV-=elapsedTV;
        sleepThread(sleepPeriodTV);
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
    bool ok = runModule();
    return ok?0:1;
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

