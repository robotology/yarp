// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <string>

#include "RosType.h"
#include "RosTypeCodeGenYarp.h"

#include <yarp/os/all.h>

using namespace yarp::os;
using namespace std;

void show_usage() {
    printf("Usage:\n");
    printf("\n  yarpidl_rosmsg <Foo>.msg\n");
    printf("    Translates a ROS-format .msg file to a YARP-compatible .h file\n");
    printf("\n  yarpidl_rosmsg <Foo>\n");
    printf("    Calls 'rosmsg' to find type Foo, then makes a .h file for it\n");
    printf("\n  yarpidl_rosmsg --out <dir> <Foo>.msg\n");
    printf("    Generates .h file in the specified directory\n");
    printf("\n  yarpidl_rosmsg <Foo>.srv\n");
    printf("    Translates a ROS-format .srv file to a pair of YARP-compatible .h files\n");
    printf("    The classes generated for Foo.srv are Foo and FooReply.\n");
}

int main(int argc, char *argv[]) {
    if (argc<=1) {
        show_usage();
        return 0;
    }
    if (std::string("help")==argv[1] || std::string("--help")==argv[1]) {
        show_usage();
        return 0;        
    }

    bool is_service = false;

    Property p;
    string fname;
    p.fromCommand(argc-1,argv);
    fname = argv[argc-1];

    if (fname.rfind(".")!=string::npos) {
        string ext = fname.substr(fname.rfind("."),fname.length());
        if (ext==".srv" || ext==".SRV") {
            is_service = true;
        }
    }
 
    RosTypeSearch env;
    RosType t;

    RosTypeCodeGenYarp gen;
    if (p.check("out")) {
        gen.setTargetDirectory(p.find("out").toString().c_str());
        env.setTargetDirectory(gen.getTargetDirectory().c_str());
    }
    env.lookForService(is_service);

    if (t.read(fname.c_str(),env,gen)) {
        RosTypeCodeGenState state;
        t.emitType(gen,state);
    }

    return 0;
}
