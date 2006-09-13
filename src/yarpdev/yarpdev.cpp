// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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

/*
class MonitorSearchable : public SearchMonitor {
public:


    virtual bool check(const char *key) {
        printf("checking %s\n", key);
        return searchable.check(key);
    }

    virtual Value& find(const char *key) {
        printf("finding %s\n", key);
        return searchable.find(key);
    }

    virtual Bottle& findGroup(const char *key) {
        printf("find group %s\n", key);
        return searchable.findGroup(key);
    }

    virtual bool isNull() const  { 
        return searchable.isNull();
    }

    virtual ConstString toString() const {
        return searchable.toString();
    }

    // self-callers
    //virtual bool check(const char *key, Value *& result);
    //virtual Value check(const char *key, const Value& fallback);
};
*/

class YarpDevMonitor : public SearchMonitor {
public:
    virtual void report(const SearchReport& report, const char *context) {
        ConstString key = report.key;
        if (key=="wrapped") {
            return;
        }
        printf("  [%s] checking option \"%s\" ",
               context, key.c_str());
        if (report.isFound) {
            printf("| found ");
            String txt = report.value.c_str();
            if (txt.length()<80) {
                printf("| %s ", txt.c_str());
            } else {
                printf("| (value is long; suppressed) ");
            }
        } else {
            printf("| not set ");
        }
        printf("\n");
    }
};

int main(int argc, char *argv[]) {

	Network::init();

    // just list the devices if no argument given
    if (argc==1) {
        printf("You can call yarpdev like this:\n");
        printf("   yarpdev --device DEVICENAME --OPTION VALUE ...\n");
        printf("For example:\n");
        printf("   yarpdev --device test_grabber --width 32 --height 16 --name /grabber\n");
        printf("or:\n");
        printf("   yarpdev --device DEVICENAME --file CONFIG_FILENAME\n");
        printf("Here are devices listed for your system:\n");
        printf("%s", Drivers::factory().toString().c_str());
        return 0;
    }

    // get command line options
    Property options;
    if (argc==2) {
        // just one value, assume it is a device name
        options.put("device",argv[1]);
    } else {
        // interpret as a set of flags
        options.fromCommand(argc,argv);
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

    // create a device
    //printf("Options: %s\n", options.toString().c_str());
    YarpDevMonitor monitor;
    options.setMonitor(&monitor,"top-level");
    PolyDriver dd(options);
    if (!dd.isValid()) {
        printf("yarpdev: ***ERROR*** device not available.\n");
        if (argc==1) { 
            printf("Here are the known devices:\n");
            printf("%s", Drivers::factory().toString().c_str());
        } else {
            printf("Call yarpdev with no arguments to see list of devices.\n");
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

