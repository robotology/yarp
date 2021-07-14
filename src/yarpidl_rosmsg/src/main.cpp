/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <string>

#include "RosType.h"
#include "RosTypeCodeGenYarp.h"

#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Type.h>
#include <yarp/os/Value.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformSysWait.h>

#include <cstdlib>

using namespace yarp::os;
using namespace std;

void show_usage()
{
    printf("Usage:\n");
    printf("\n");
    printf("  yarpidl_rosmsg [OPTIONS] [<Foo>.msg|<package>/<Foo>]\n");
    printf("    Translates a ROS-format .msg file to a YARP-compatible .h file\n");
    printf("  yarpidl_rosmsg [OPTIONS] [<Foo>.srv]\n");
    printf("    Translates a ROS-format .srv file to a pair of YARP-compatible .h files\n");
    printf("    The classes generated for Foo.srv are Foo and FooReply.\n");
    printf("  yarpidl_rosmsg --name [/name]\n");
    printf("    Start up a service with the given port name for querying types.\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  --no-ros true\n");
    printf("    Do not try to fetch missing types from rosmsg/rossrv\n");
    printf("  --web true\n");
    printf("    Allow YARP to look up missing types on ROS website\n");
    printf("  --out <dir>\n");
    printf("    Generates .h file in the specified directory\n");
    printf("  --no-cache\n");
    printf("    Do not cache ros msg file\n");
    printf("  --no-index\n");
    printf("    Do not generate the index.txt file\n");
    printf("  --verbose\n");
    printf("    Verbose output\n");
    printf("\n");
}

static void generateTypeMap1(RosType& t, std::string& txt)
{
    if (!t.isValid) {
        return;
    }
    std::vector<RosType>& lst = t.subRosType;
    if (lst.size()>0) {
        bool simple = true;
        for (auto& type : lst) {
            if (type.rosType != lst[0].rosType ||
                (!type.isPrimitive) ||
                type.isArray) {
                simple = false;
                break;
            }

        }
        if (!simple) {
            txt += " list ";
            txt += yarp::conf::numeric::to_string((int)lst.size());

            for (auto& i : lst) {
                generateTypeMap1(i, txt);
            }
        } else {
            txt += " vector ";
            txt += lst[0].rosType;
            txt += " ";
            txt += yarp::conf::numeric::to_string((int)lst.size());
            txt += " *";
        }
        return;
    }
    if (!t.isPrimitive) {
        return;
    }
    txt += " ";
    if (t.isArray) {
        txt += "vector ";
        txt += t.rosType;
    } else {
        txt += t.rosType;
    }
    txt += " *";
}

static void generateTypeMap(RosType& t, std::string& txt)
{
    txt = "";
    generateTypeMap1(t,txt);
    if (txt.length()>0) {
        txt = txt.substr(1,txt.length());
    }
    if (!t.reply) {
        return;
    }
    txt += " ---";
    generateTypeMap1(*(t.reply),txt);
}

void configure_search(RosTypeSearch& env, Searchable& p)
{
    if (p.check("out")) {
        env.setTargetDirectory(p.find("out").toString().c_str());
    }
    if (p.check("no-ros",Value(0)).asInt32()!=0 || p.findGroup("no-ros").size()==1) {
        env.disableRos();
    }
    if (p.check("web",Value(0)).asInt32()!=0 || p.findGroup("web").size()==1) {
        env.enableWeb();
    }
    if (p.check("soft",Value(0)).asInt32()!=0 || p.findGroup("soft").size()==1) {
        env.softFail();
    }
    env.lookForService(p.check("service"));
}

int generate_cpp(int argc, char *argv[])
{
    bool is_service = false;

    Property p;
    string fname;
    p.fromCommand(argc,argv);
    bool verbose = p.check("verbose");
    bool no_cache = p.check("no-cache");
    bool no_index = p.check("no-index");

    fname = argv[argc-1];

    if (fname.rfind('.')!=string::npos) {
        string ext = fname.substr(fname.rfind('.'),fname.length());
        if (ext==".srv" || ext==".SRV") {
            is_service = true;
        }
    }
    if (is_service) {
        p.put("service",1);
    }

    RosTypeSearch env;
    RosType t;
    RosTypeCodeGenYarp gen;
    if (verbose) {
        env.setVerbose();
        t.setVerbose();
        gen.setVerbose();
    }

    if (p.check("out")) {
        gen.setTargetDirectory(p.find("out").toString().c_str());
    }
    configure_search(env,p);

    if (t.read(fname.c_str(),env,gen)) {
        RosTypeCodeGenState state;
        t.emitType(gen,state);
        if (!no_cache) {
            t.cache(fname.c_str(),env,gen);
        }
        if (!no_index) {
            gen.writeIndex(state);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc<=1) {
        show_usage();
        return 0;
    }
    if (std::string("help")==argv[1] || std::string("--help")==argv[1]) {
        show_usage();
        return 0;
    }

    Property p;
    p.fromCommand(argc,argv);
    bool verbose = p.check("verbose");

    if (verbose)
    {
        printf("Command line: ");
        for (int c = 0; c < argc; c++)
        {
            printf("%s ", argv[c]);
        }
        printf("\n");
    }

    if (!(p.check("name")||p.check("cmd"))) {
        return generate_cpp(argc,argv);
    }
    if (!p.check("soft")) {
        p.put("soft",1);
    }
    if (!p.check("web")) {
        p.put("web",1);
    }

    bool has_cmd = p.check("cmd");

    RosTypeSearch env;
    configure_search(env,p);

    Network yarp(YARP_CLOCK_SYSTEM);
    Port port;
    if (!has_cmd) {
        // Borrow an accidentally-available service type on ROS, in
        // order to avoid adding build dependencies for now.
        port.promiseType(Type::byNameOnWire("test_roscpp/TestStringString"));
        port.setRpcServer();
        if (!port.open(p.find("name").asString())) {
            return 1;
        }
    }
    while (true) {
        Bottle req;
        if (has_cmd) {
            req = p.findGroup("cmd").tail();
        } else {
            if (!port.read(req,true)) {
                continue;
            }
        }
        if (req.size()==1) {
            req.fromString(req.get(0).asString());
        }
        if (verbose) {
            printf("Request: %s\n", req.toString().c_str());
        }
        Bottle resp;
        std::string tag = req.get(0).asString();
        string fname0 = req.get(1).asString();
        string fname = env.findFile(fname0.c_str());
        string txt;
        if (tag=="raw") {
            txt = env.readFile(fname.c_str());
            resp.addString(txt);
        } else if (tag=="twiddle") {
            RosTypeCodeGenYarp gen;
            RosType t;
            if (t.read(fname0.c_str(),env,gen)) {
                std::string txt;
                generateTypeMap(t,txt);
                resp.addString(txt);
            } else {
                resp.addString("?");
            }
        } else {
            resp.addString("?");
        }
        if (!has_cmd) {
            port.reply(resp);
        }
        if (verbose||has_cmd) {
            printf("Response: %s\n", resp.toString().c_str());
        }
        if (has_cmd) {
            break;
        }
    }
    return 0;
}
