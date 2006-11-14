// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/String.h>

#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Terminator.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;
using namespace std;


static void toDox(PolyDriver& dd, ostream& os) {
    os << "===============================================================" 
       << endl;
    os << "== Options checked by device:" << endl;
    os << "== " << endl;

    Bottle order = dd.getOptions();
    for (int i=0; i<order.size(); i++) {
        String name = order.get(i).toString().c_str();
        if (name=="wrapped"||(name.strstr(".wrapped")>=0)) {
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
        os << out.c_str() << endl;
    }
    os << "==" << endl;
    os << "===============================================================" 
       << endl;
}

int main(int argc, char *argv[]) {

	Network::init();

    // get command line options
    Property options;

    // yarpdev will by default try to pass its thread on to the device.
    // this is because some libraries need configuration and all
    // access methods done in the same thread (e.g. opencv_grabber
    // apparently).
    options.put("single_threaded", 1);

    // interpret as a set of flags
    options.fromCommand(argc,argv,true,false);

    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file",val)) {
        ConstString fname = val->toString();
        options.unput("file");
        printf("yarpdev: working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname,false);
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
    PolyDriver dd(options);
    if (verbose) {
        toDox(dd,cout);
    }
    if (!dd.isValid()) {
        printf("yarpdev: ***ERROR*** device not available.\n");
        if (argc==1) { 
            printf("Here are the known devices:\n");
            printf("%s", Drivers::factory().toString().c_str());
        } else {
            printf("Suggestions:\n");
            printf("+ Do \"yarpdev --list\" to see list of supported devices.\n");
            printf("+ Or append \"--verbose\" option to get more information.\n");
        }
        return 1;
    }

    Terminee *terminee = 0;
    if (dd.isValid()) {
        Value *v;
        // default to /argv[0]/quit
        yarp::String s("/");
        s += argv[0];
        s += "/quit";
        if (options.check("name", v)) {
            s.clear();
            s += v->toString().c_str();
            s += "/quit";
        }
        terminee = new Terminee(s.c_str());
        if (terminee == 0) {
            printf("Can't allocate terminator socket port\n");
            return 1;
        }
        if (!terminee->isOk()) {
            printf("Failed to create terminator socket port\n");
            return 1;
        }
    }

    double startTime = Time::now();
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
    while (dd.isValid() && !terminee->mustQuit()) {
        if (service!=NULL) {
            double now = Time::now();
            double dnow = 1;
            if (now-startTime>dnow) {
                printf("yarpdev: device active...\n");
                startTime += dnow;
            }
            // we requested single threading, so need to
            // give the device its chance
            service->updateService();
        } else {
            // we don't need to do anything
            printf("yarpdev: device active in background...\n");
            Time::delay(1);
        }
    }

    delete terminee;
    dd.close();
	Network::fini();

    return 0;
}

