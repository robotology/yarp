// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include "TestList.h"

#include <fstream>
#include <iostream>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;

using namespace std;

#ifdef YARP2_LINUX
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#include <mcheck.h>
#endif


int harness_main(int argc, char *argv[]) {
#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    Network yarp;

    bool done = false;
    int result = 0;

    if (argc>1) {
        int verbosity = 0;
        while (String(argv[1])==String("verbose")) {
            verbosity++;
            argc--;
            argv++;
        }
        if (verbosity>0) {
            Logger::get().setVerbosity(verbosity);
        }
    
        if (String(argv[1])==String("regression")) {
            done = true;
            UnitTest::startTestSystem();
            TestList::collectTests();  // just in case automation doesn't work
            if (argc>2) {
                result = UnitTest::getRoot().run(argc-2,argv+2);
            } else {
                result = UnitTest::getRoot().run();
            }
            UnitTest::stopTestSystem();
            NameClient::removeNameClient();
        }
    } 
    if (!done) {
        Companion::main(argc,argv);
    }
    ACE::fini();

    return result;
}



static String getFile(const char *fname) {
    ifstream fin(fname);
    String txt;
    if (fin.fail()) {
        return "";
    }
    while (!(fin.eof()||fin.fail())) {
        char buf[1000];
        fin.getline(buf,sizeof(buf));
        if (!fin.eof()) {
            txt += buf;
            txt += "\n";
        }
    }
    return txt;
}


static void toDox(PolyDriver& dd, ostream& os) {
    os << "<table>" << endl;
    os << "<tr><td>PROPERTY</td><td>DESCRIPTION</td><td>DEFAULT</td></tr>"
       << endl;
    Bottle order = dd.getOptions();
    for (int i=0; i<order.size(); i++) {
        String name = order.get(i).toString().c_str();
        if (name=="wrapped"||name.substr(0,10)=="subdevice.") {
            continue;
        }
        ConstString desc = dd.getComment(name.c_str());
        ConstString def = dd.getDefaultValue(name.c_str()).toString();
        String out = "";
        out += "<tr><td>";
        out += name.c_str();
        out += "</td><td>";
        out += desc.c_str();
        out += "</td><td>";
        out += def.c_str();
        out += "</td></tr>";
        os << out.c_str() << endl;
    }
    os << "</table>" << endl;
}

int main(int argc, char *argv[]) {
    Property p;
    p.fromCommand(argc,argv);

    // check where to put description of device
    ConstString dest = "";
    dest = p.check("doc",Value("")).toString();

    ConstString fileName = p.check("file",Value("default.ini")).asString();

    if (p.check("file")) {
        p.fromConfigFile(fileName);
    }

    ConstString deviceName = p.check("device",Value("")).asString();

    // if no device given, we should be operating a completely
    // standard test harness like for libYARP_OS and libYARP_sig
    if (deviceName=="") {
        return harness_main(argc,argv);
    }

    // device name given - use special device testing procedure

#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    int result = 0;

    Network::init();
    Network::setLocalMode(true);

    String seek = fileName.c_str();
    ConstString exampleName = "";
    int pos = seek.rfind('/');
    if (pos==-1) {
        pos = seek.rfind('\\');
    }
    if (pos==-1) {
        pos = 0;
    } else {
        pos++;
    }
    int len = seek.find('.',pos);
    if (len==-1) {
        len = seek.length();
    } else {
        len -= pos;
    }
    exampleName = seek.substr(pos,len).c_str();
    ConstString shortFileName = seek.substr(pos,seek.length()).c_str();

    PolyDriver dd;
	YARP_DEBUG(Logger::get(), "harness opening...");
	
    bool ok = dd.open(p);
    YARP_DEBUG(Logger::get(), "harness opened.");
    result = ok?0:1;

    ConstString wrapperName = "";
    ConstString codeName = "";

    DriverCreator *creator = 
        Drivers::factory().find(deviceName.c_str());
    if (creator!=NULL) {
        wrapperName = creator->getWrapper();
        codeName = creator->getCode();
    }


    if (dest!="") {
        ofstream fout(dest.c_str());
        if (!fout.good()) {
            printf("Problem writing to %s\n", dest.c_str());
            exit(1);
        }
        fout << "/**" << endl;
        fout << " * \\ingroup dev_examples" << endl;
        fout << " *" << endl;
        fout << " * \\defgroup " 
             << exampleName.c_str() 
             << " Example for " 
             << deviceName.c_str()
             << " ("
             << exampleName.c_str() 
             << ")"
             << endl;
        fout << endl;
        fout << "Instantiates "
             << "\\ref cmd_device_"
             << deviceName.c_str()
             << " \""
             << deviceName.c_str()
             << "\""
             << " device implemented by yarp::dev::"
             << codeName.c_str()
             << "."
             << endl;
        fout << "\\verbatim" << endl;
        fout << getFile(fileName).c_str();
        fout << "\\endverbatim" << endl;
        fout << endl;
        fout << "If this text is saved in a file called "
             << shortFileName.c_str()
             << " then the device can be created by doing:"
             << endl;
        fout << "\\verbatim" << endl;
        fout << "yarpdev "
             << "--file "
             << shortFileName.c_str()
             << endl;
        fout << "\\endverbatim" << endl;
        fout << "Of course, the configuration could be passed just as command line options, or as a yarp::os::Property object in a program:" << endl;
        fout << endl;
        fout << "\\code" << endl;
        fout << "Property p;" << endl;
        fout << "p.fromConfigFile(\"" << shortFileName.c_str() << "\");" << endl;
        fout << "// of course you could construct the Property object on-the-fly" << endl;
        fout << "PolyDriver dev;" << endl;
        fout << "dev.open(p);" << endl;
        fout << "if (dev.isValid()) { /* use the device via view method */ }" 
             << endl;
        fout << "\\endcode" << endl;
        fout << "Here is a list of properties checked when starting up a device based on this configuration file.  Note that which properties are checked can depend on whether other properties are present.  In some cases properties can also vary between operating systems.  So this is just an example" << endl;
        fout << endl;
        toDox(dd,fout);
        fout << endl;
        fout << "\\sa ";
        fout << "yarp::dev::"
             << codeName.c_str()
             << endl;
        fout << endl;
        fout << " */" << endl;
        fout.close();
    }


    if (ok) {
        YARP_DEBUG(Logger::get(), "harness closing...");
        dd.close();
        YARP_DEBUG(Logger::get(), "harness closed.");
    }

    // just checking for crashes, not device creation
    return 0; //result;
}

