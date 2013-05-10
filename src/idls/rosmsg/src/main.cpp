// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Platform
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

void show_usage() {
    printf("Usage:\n");
    printf("\n  yarpidl_rosmsg <Foo>.msg\n");
    printf("    Translates a ROS-format .msg file to a YARP-compatible .h file\n");
    printf("\n  yarpidl_rosmsg <Foo>\n");
    printf("    Calls 'rosmsg' to find type Foo, then makes a .h file for it\n");
    printf("\n  yarpidl_rosmsg --out <dir> <Foo>.msg\n");
    printf("    Generates .h file in the specified directory\n");
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

    Property p;
    ConstString fname;
    p.fromCommand(argc-1,argv);
    fname = argv[argc-1];
 
    RosTypeSearch env;
    RosType t;

    RosTypeCodeGenYarp gen;
    if (p.check("out")) {
        gen.setTargetDirectory(p.find("out").toString().c_str());
    }

    if (t.read(fname,env,gen)) {
        RosTypeCodeGenState state;
        t.emitType(gen,state);
    }

    return 0;
}
