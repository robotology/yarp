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

#include <stdio.h>
#include <stdlib.h>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;


class YarpDevMonitor : public SearchMonitor {
private:
    ConstString prev;
public:
    YarpDevMonitor() {
        prev = "";
    }

    virtual void report(const SearchReport& report, const char *context) {
        ConstString key = report.key;
        if (key=="wrapped") {
            return;
        }

        String prefix = "*** ";
        prefix += key.c_str();
        prefix += ":";
        if (key==prev) {
            for (int i=0; i<prefix.length(); i++) {
                if (prefix[i]=='*') {
                    prefix[i] = ' ';
                }
            }
        }
        prev = key;

        if (report.isComment==true) {
            printf("  [%s] %s \"%s\"\n",
                   context, prefix.c_str(), report.value.c_str());
            return;
        }

        if (report.isDefault==true) {
            printf("  [%s] %s  default %s\n",
                   context, prefix.c_str(), report.value.c_str());
            return;
        }

        if (report.isFound) {
            printf("  [%s] %s ",
                   context, prefix.c_str());
            String txt = report.value.c_str();
            if (txt.length()<80) {
                printf(" %s", txt.c_str());
            } else {
                printf(" (value is long; suppressed)");
            }
            printf("\n");
        } else {
            printf("  [%s] %s  not found\n",
                   context, prefix.c_str());
        }
    }
};

int main(int argc, char *argv[]) {

	Network::init();

    // get command line options
    Property options;
    // interpret as a set of flags
    options.fromCommand(argc,argv);

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

    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file",val)) {
        Value *v1 = 0, *v2 = 0;
        options.check("device", v1);
        options.check("subdevice", v2);
        ConstString dname = (v1)?v1->toString():"";
        ConstString sdname = (v2)?v2->toString():"";
        ConstString fname = val->toString();
        printf("yarpdev: working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname);
        if (v1)
            options.put("device", dname.c_str());
        if (v2)
            options.put("subdevice", sdname.c_str());
    }

    // check if we want to use nested options (less ambiguous)
    if (options.check("nested",val)||options.check("lispy",val)) {
        ConstString lispy = val->toString();
        printf("yarpdev: working with config %s\n", lispy.c_str());
        options.fromString(lispy);
    }

    // ask for a wrapped, remotable device rather than raw device
    options.put("wrapped","1");

    YarpDevMonitor monitor;
    if (options.check("verbose")) {
        options.setMonitor(&monitor,"top-level");
    }
    PolyDriver dd(options);
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

    while (dd.isValid() && !terminee->mustQuit()) {
        printf("yarpdev: device active...\n");
        Time::delay(1);
    }

    delete terminee;
    dd.close();
	Network::fini();

    return 0;
}

