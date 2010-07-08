// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Terminator.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>

#include <yarp/dev/Drivers.h>

//#include <ace/OS.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h> 
#include <ace/OS_NS_signal.h>
#include <ace/Vector_T.h>
#include <stdio.h>
#include <stdlib.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

Drivers Drivers::instance;

class DriversHelper {
public:
    ACE_Vector<DriverCreator *> delegates;

    ~DriversHelper() {
        for (unsigned int i=0; i<delegates.size(); i++) {
            if (delegates[i]==NULL) continue;
            delete delegates[i];
        }
        delegates.clear();
    }

    ConstString toString() {
        String s;
        for (unsigned int i=0; i<delegates.size(); i++) {
            if (delegates[i]==NULL) continue;
            ConstString name = delegates[i]->getName();
            ConstString wrapper = delegates[i]->getWrapper();
            s += "Device \"";
            s += delegates[i]->getName().c_str();
            s += "\"";
            s += ",";
            s += " C++ class ";
            s += delegates[i]->getCode().c_str();
            s += ", ";
            if (wrapper=="") {
                s += "has no network wrapper";
            } else if (wrapper!=name) {
                s += "wrapped by \"";
                s += delegates[i]->getWrapper().c_str();
                s += "\"";
            } else {
                s += "is a network wrapper.";
            }
            s += "\n";
        }
        return ConstString(s.c_str());
    }

    void add(DriverCreator *creator) {
        if (creator!=NULL) {
            delegates.push_back(creator);
        }
    }

    DriverCreator *find(const char *name) {
        for (unsigned int i=0; i<delegates.size(); i++) {
            if (delegates[i]==NULL) continue;
            String s = delegates[i]->toString().c_str();
            if (s==name) {
                return delegates[i];
            }
        }
        return NULL;
    }

    bool remove(const char *name) {
        for (unsigned int i=0; i<delegates.size(); i++) {
            if (delegates[i]==NULL) continue;
            String s = delegates[i]->toString().c_str();
            if (s==name) {
                delete delegates[i];
                delegates[i] = NULL;
            }
        }
        return false;
    }
};

#define HELPER(x) (*(((DriversHelper*)(x))))

Drivers::Drivers() {
    implementation = new DriversHelper;
    YARP_ASSERT(implementation!=NULL);
    init();
}


Drivers::~Drivers() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}

yarp::os::ConstString Drivers::toString() {
    return HELPER(implementation).toString();
}

void Drivers::add(DriverCreator *creator) {
    HELPER(implementation).add(creator);
}


DriverCreator *Drivers::find(const char *name) {
    return HELPER(implementation).find(name);
}

bool Drivers::remove(const char *name) {
    return HELPER(implementation).remove(name);
}


DeviceDriver *Drivers::open(yarp::os::Searchable& prop) {
    PolyDriver poly;
    bool result = poly.open(prop);
    if (!result) return NULL;
    return poly.take();
}


// helper method for "yarpdev" body
static void toDox(PolyDriver& dd, FILE *os) {
    fprintf(os,"===============================================================\n");
    fprintf(os,"== Options checked by device:\n== \n");

    Bottle order = dd.getOptions();
    for (int i=0; i<order.size(); i++) {
        String name = order.get(i).toString().c_str();
        if (name=="wrapped"||(YARP_STRSTR(name,".wrapped")!=String::npos)) {
            continue;
        }
        ConstString desc = dd.getComment(name.c_str());
        Value def = dd.getDefaultValue(name.c_str());
        Value actual = dd.getValue(name.c_str());
        String out = "";
        out += name.c_str();
        if (!actual.isNull()) {
            if (actual.toString()!="") {
                out += "=";
                if (actual.toString().length()<40) {
                    out += actual.toString().c_str();
                } else {
                    out += "(value too long)";
                }
            }
        }
        if (!def.isNull()) {
            if (def.toString()!="") {
                out += " [";
                if (def.toString().length()<40) {
                    out += def.toString().c_str();
                } else {
                    out += "(value too long)";
                }
                out += "]";
            }
        }
        if (desc!="") {
            out += "\n    ";
            out += desc.c_str();
        }
        fprintf(os,"%s\n", out.c_str());
    }
    fprintf(os,"==\n");
    fprintf(os,"===============================================================\n");
}


static ConstString terminatorKey = "";
static bool terminated = false;
static void handler (int) {
    static double handleTime = -100;
    static int ct = 0;
    double now = Time::now();
    if (now-handleTime<1) {
        return;
    }
    handleTime = now;
    ct++;
    if (ct>3) {
        printf("Aborting...\n");
        ACE_OS::exit(1);
    }
    if (terminatorKey!="") {
        printf("[try %d of 3] Trying to shut down %s\n", 
               ct,
               terminatorKey.c_str());
        terminated = true;
        Terminator::terminateByName(terminatorKey.c_str());
    } else {
        printf("Aborting...\n");
        ACE_OS::exit(1);
    }
}


int Drivers::yarpdev(int argc, char *argv[]) {

    ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) handler);

    // get command line options
    Property options;

    // yarpdev will by default try to pass its thread on to the device.
    // this is because some libraries need configuration and all
    // access methods done in the same thread (e.g. opencv_grabber
    // apparently).
    options.put("single_threaded", 1);

    // interpret command line options as a set of flags
    options.fromCommand(argc,argv,true,false);

    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file",val)) {
        ConstString fname = val->toString();
        options.unput("file");
        printf("yarpdev: working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname,false);

        // interpret command line options as a set of flags again
        // (just in case we need to override something)
        options.fromCommand(argc,argv,true,false);
    }

    // check if we want to use nested options (less ambiguous)
    if (options.check("nested",val)||options.check("lispy",val)) {
        ConstString lispy = val->toString();
        printf("yarpdev: working with config %s\n", lispy.c_str());
        options.fromString(lispy);
    }

    if (!options.check("device")) {
        // no device mentioned - maybe user needs help
        
        if (options.check("list")) {
            printf("Here are devices listed for your system:\n");
            printf("%s", Drivers::factory().toString().c_str());
        } else {
            printf("Welcome to yarpdev, a program to create YARP devices\n");
            printf("To see the devices available, try:\n");
            printf("   yarpdev --list\n");
            printf("To create a device whose name you know, call yarpdev like this:\n");
            printf("   yarpdev --device DEVICENAME --OPTION VALUE ...\n");
            printf("For example:\n");
            printf("   yarpdev --device test_grabber --width 32 --height 16 --name /grabber\n");
            printf("You can always move options to a configuration file:\n");
            printf("   yarpdev [--device DEVICENAME] --file CONFIG_FILENAME\n");
            printf("If you have problems, you can add the \"verbose\" flag to get more information\n");
            printf("   yarpdev --verbose --device ffmpeg_grabber\n");
        }
        return 0;
    }

    // ask for a wrapped, remotable device rather than raw device
    options.put("wrapped","1");

    //YarpDevMonitor monitor;
    bool verbose = false;
    if (options.check("verbose")) {
        verbose = true;
        //options.setMonitor(&monitor,"top-level");
    }

    // we now need network
    bool ret=Network::checkNetwork();
    if (!ret)
    {
        fprintf(stderr, "YARP network not available, chech if yarp server is reachable\n");
        return -1;
    }

    PolyDriver dd(options);
    if (verbose) {
        toDox(dd,stdout);
    }
    if (!dd.isValid()) {
        printf("yarpdev: ***ERROR*** device not available.\n");
        if (argc==1) { 
            printf("Here are the known devices:\n");
            printf("%s", Drivers::factory().toString().c_str());
        } else {
            printf("Suggestions:\n");
            printf("+ Do \"yarpdev --list\" to see list of supported devices.\n");
            if (!options.check("verbose")) {
                printf("+ Or append \"--verbose\" option to get more information.\n");
            }
        }
        return 1;
    }

    Terminee *terminee = 0;
    if (dd.isValid()) {
        Value *v;
        String s("/yarpdev/quit");
        if (options.check("device", v)) {
            if (v->isString()) {
                s.clear();
                s += "/";
                s += v->toString().c_str();
                s += "/quit";
            }
        }
        if (options.check("name", v)) {
            s.clear();
            s += v->toString().c_str();
            s += "/quit";
        }
        terminee = new Terminee(s.c_str());
        terminatorKey = s.c_str();
        if (terminee == 0) {
            printf("Can't allocate terminator port\n");
            terminatorKey = "";
            dd.close();
            return 1;
        }
        if (!terminee->isOk()) {
            printf("Failed to create terminator port\n");
            terminatorKey = "";
            delete terminee;
            terminee = NULL;
            dd.close();
            return 1;
        }
    }

    double dnow = 3;
    double startTime = Time::now()-dnow;
    IService *service = NULL;
    dd.view(service);
    if (service!=NULL) {
        bool backgrounded = service->startService();
        if (backgrounded) {
            // we don't need to poll this, so forget about the
            // service interface
            printf("yarpdev: service backgrounded\n");
            service = NULL;
        }
    }
    while (dd.isValid() && !(terminated||terminee->mustQuit())) {
        if (service!=NULL) {
            double now = Time::now();
            if (now-startTime>dnow) {
                printf("yarpdev: device active...\n");
                fflush(stdout);
                startTime += dnow;
            }
            // we requested single threading, so need to
            // give the device its chance
            service->updateService();
        } else {
            // we don't need to do anything
            printf("yarpdev: device active in background...\n");
            fflush(stdout);
            Time::delay(dnow);
        }
    }

    delete terminee;
    dd.close();

    printf("yarpdev is finished.\n");

    return 0;
}





// defined in PopulateDrivers.cpp:
//   DeviceDriver *Drivers::init()



