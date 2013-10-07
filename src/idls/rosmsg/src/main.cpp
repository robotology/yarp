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

#include <sys/stat.h>

#ifdef YARP_PRESENT
#  include <yarp/conf/system.h>
#endif
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_unistd.h>
#  include <ace/OS_NS_sys_wait.h>
#else
#  include <unistd.h>
#  include <sys/wait.h>
#  ifndef ACE_OS
#    define ACE_OS
#  endif
#endif
#include <stdlib.h>

using namespace yarp::os;
using namespace std;

void show_usage() {
    printf("Usage:\n");
    printf("\n  yarpidl_rosmsg <Foo>.msg\n");
    printf("    Translates a ROS-format .msg file to a YARP-compatible .h file\n");
    printf("\n  yarpidl_rosmsg <package>/<Foo>\n");
    printf("    Calls 'rosmsg' to find type Foo, then makes a .h file for it\n");
    printf("\n  yarpidl_rosmsg --web <package>/<Foo>\n");
    printf("    Allow YARP to look up missing types on ROS website\n");
    printf("\n  yarpidl_rosmsg --out <dir> <Foo>.msg\n");
    printf("    Generates .h file in the specified directory\n");
    printf("\n  yarpidl_rosmsg <Foo>.srv\n");
    printf("    Translates a ROS-format .srv file to a pair of YARP-compatible .h files\n");
    printf("    The classes generated for Foo.srv are Foo and FooReply.\n");
    printf("\n  yarpidl_rosmsg --name /name\n");
    printf("    Start up a service with the given port name for querying types.\n");
}

int generate_cpp(int argc, char *argv[]) {
    bool is_service = false;

    Property p;
    string fname;
    p.fromCommand(argc,argv);

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
    if (p.check("web")) {
        env.allowWeb();
    }
    env.lookForService(is_service);

    if (t.read(fname.c_str(),env,gen)) {
        RosTypeCodeGenState state;
        t.emitType(gen,state);
    }

    return 0;
}

string lookup_type(string name, bool find_service) {
    string cache_file = "/tmp/yarpidl_rosmsg_lookup_cache";
    string cmd = string(find_service?"rossrv":"rosmsg") + " show -r " + name + " > " + cache_file;
    fprintf(stderr,"[ros]  %s\n", cmd.c_str());
    pid_t p = ACE_OS::fork();
    if (p==0) {
#ifdef __linux__
        // This was ACE_OS::execlp, but that fails
        ::execlp("sh","sh","-c",cmd.c_str(),(char *)NULL);
#else
        ACE_OS::execlp("sh","sh","-c",cmd.c_str(),(char *)NULL);
#endif
        ::exit(0);
    } else {
        ACE_OS::wait(NULL);
    }
    FILE *fin = fopen(cache_file.c_str(),"r");
    string txt = "";
    if (fin) {
        char *result = NULL;
        do {
            char buf[2048];
            result = fgets(buf,sizeof(buf),fin);
            if (result==NULL) break;
            txt += result;
        } while (result!=NULL);
    }
    return txt;
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
    p.fromCommand(argc,argv);

    if (!p.check("name")) {
        return generate_cpp(argc,argv);
    }

    Network yarp;
    Port port;
    if (!port.open(p.find("name").asString())) {
        return 1;
    }
    while (true) {
        Bottle req;
        port.read(req,true);
        Bottle unpack_req;
        if (req.size()==1) {
            unpack_req.fromString(req.get(0).asString());
        } else {
            unpack_req = req;
        }
        Bottle resp;
        ConstString tag = req.get(0).asString();
        if (tag=="raw") {
            resp.addString(lookup_type(req.get(1).asString().c_str(),false));
        } else {
            resp.addString("?");
        }
        port.reply(resp);
    }
    return 0;
}
