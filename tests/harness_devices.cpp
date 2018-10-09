/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "YarpBuildLocation.h"

#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;

using namespace std;


static std::string getFile(const char *fname)
{
    char buf[25600];
    FILE *fin = fopen(fname, "r");
    if (fin==nullptr) return "";
    std::string result = "";
    while(fgets(buf, sizeof(buf)-1, fin) != nullptr) {
        result += buf;
    }
    fclose(fin);
    fin = nullptr;
    return result;
}


static void toDox(PolyDriver& dd, FILE *os)
{
    fprintf(os, "<table>\n");
    fprintf(os, "<tr><td>PROPERTY</td><td>DESCRIPTION</td><td>DEFAULT</td></tr>\n");
    Bottle order = dd.getOptions();
    for (size_t i=0; i<order.size(); i++) {
        std::string name = order.get(i).toString().c_str();
        if (name=="wrapped"||name.substr(0,10)=="subdevice.") {
            continue;
        }
        std::string desc = dd.getComment(name.c_str());
        std::string def = dd.getDefaultValue(name.c_str()).toString();
        std::string out = "";
        out += "<tr><td>";
        out += name.c_str();
        out += "</td><td>";
        out += desc.c_str();
        out += "</td><td>";
        out += def.c_str();
        out += "</td></tr>";
        fprintf(os, "%s\n",out.c_str());
    }
    fprintf(os, "</table>\n");
}

void usage(const char* name)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    %s --doc <destination> --file <file>\n", name);
    fprintf(stderr, "    %s --doc <destination> --device <device> [device options]\n", name);
    fprintf(stderr, "\n");
}


int main(int argc, char *argv[])
{
    Property p;
    p.fromCommand(argc,argv);

    // Check where to put description of device
    std::string dest = p.check("doc", Value("")).toString();
    if (dest.empty()) {
        yError("No destination specified");
        usage(argv[0]);
        return 1;
    }

    // Check if the "file" option was specified, and eventually read the file
    std::string fileName = p.check("file", Value("default.ini")).asString();
    if (p.check("file")) {
        p.fromConfigFile(fileName);
    }

    // Check the name of the device
    std::string deviceName = p.check("device", Value("")).asString();
    if (deviceName == "") {
        yError("No device specified");
        usage(argv[0]);
        return 1;
    }

    // To make sure that the dev test are able to find all the devices
    // compile by YARP, also the one compiled as dynamic plugins
    // we add the build directory to the YARP_DATA_DIRS enviromental variable
    // CMAKE_CURRENT_DIR is the define by the CMakeLists.txt tests file
    std::string dirs = CMAKE_BINARY_DIR +
                       yarp::os::Network::getDirectorySeparator() +
                       "share" +
                       yarp::os::Network::getDirectorySeparator() +
                       "yarp";

    // Add TEST_DATA_DIR to YARP_DATA_DIRS in order to find the contexts used
    // by the tests
    dirs += yarp::os::Network::getPathSeparator() +
            TEST_DATA_DIR;

    // If set, append user YARP_DATA_DIRS
    // FIXME check if this can be removed
    Network::getEnvironment("YARP_DATA_DIRS");
    if (!Network::getEnvironment("YARP_DATA_DIRS").empty()) {
        dirs += yarp::os::Network::getPathSeparator() +
                Network::getEnvironment("YARP_DATA_DIRS");
    }

    Network::setEnvironment("YARP_DATA_DIRS", dirs);
    yInfo("YARP_DATA_DIRS=\"%s\"\n", Network::getEnvironment("YARP_DATA_DIRS").c_str());


    Network::init();
    Network::setLocalMode(true);

    std::string seek = fileName.c_str();
    std::string exampleName = "";
    int pos = seek.rfind('/');
    if (pos == -1) {
        pos = seek.rfind('\\');
    }
    if (pos == -1) {
        pos = 0;
    } else {
        pos++;
    }
    int len = seek.find('.', pos);
    if (len == -1) {
        len = seek.length();
    } else {
        len -= pos;
    }
    exampleName = seek.substr(pos, len).c_str();
    std::string shortFileName = seek.substr(pos, seek.length()).c_str();

    PolyDriver dd;
    yDebug("Opening PolyDriver for device %s", deviceName.c_str());
    if (!dd.open(p)) {
        yDebug("PolyDriver failed to opened.");
        usage(argv[0]);
        return 1;
    }

    yDebug("PolyDriver opened.");

    std::string wrapperName = "";
    std::string codeName = "";

    DriverCreator *creator = Drivers::factory().find(deviceName.c_str());

    if (creator!=nullptr) {
        wrapperName = creator->getWrapper();
        codeName = creator->getCode();
    }

    FILE *fout = fopen(dest.c_str(), "w");
    if (fout == nullptr) {
        printf("Problem writing to %s\n", dest.c_str());
        std::exit(1);
    }

    fprintf(fout, "/**\n");
    fprintf(fout, " * \\ingroup dev_examples\n");
    fprintf(fout, " *\n");
    fprintf(fout, " * \\defgroup %s Example for %s (%s)\n\n",
            exampleName.c_str(),
            deviceName.c_str(),
            exampleName.c_str());
    fprintf(fout, "Instantiates \\ref cmd_device_%s \"%s\" device implemented by %s.\n",
            deviceName.c_str(), deviceName.c_str(), codeName.c_str());
    fprintf(fout, "\\verbatim\n%s\\endverbatim\n",
            getFile(fileName.c_str()).c_str());
    fprintf(fout, "If this text is saved in a file called %s then the device can be created by doing:\n",
            shortFileName.c_str());
    fprintf(fout, "\\verbatim\nyarpdev --file %s\n\\endverbatim\n",
            shortFileName.c_str());
    fprintf(fout, "Of course, the configuration could be passed just as command line options, or as a yarp::os::Property object in a program:\n");
    fprintf(fout, "\\code\n");
    fprintf(fout, "Property p;\n");
    fprintf(fout, "p.fromConfigFile(\"%s\");\n",
            shortFileName.c_str());
    fprintf(fout, "// of course you could construct the Property object on-the-fly\n");
    fprintf(fout, "PolyDriver dev;\n");
    fprintf(fout, "dev.open(p);\n");
    fprintf(fout, "if (dev.isValid()) { /* use the device via view method */ }\n" );
    fprintf(fout, "\\endcode\n");
    fprintf(fout, "Here is a list of properties checked when starting up a device based on this configuration file.\n");
    fprintf(fout, "Note that which properties are checked can depend on whether other properties are present.\n");
    fprintf(fout, "In some cases properties can also vary between operating systems.\n");
    fprintf(fout, "So this is just an example\n\n");

    toDox(dd, fout);

    fprintf(fout, "\n\\sa %s\n\n",
            codeName.c_str());
    fprintf(fout, " */\n");

    fclose(fout);
    fout = nullptr;

    yDebug("Closing PolyDriver");
    dd.close();
    yDebug("PolyDriver closed.");

    return 0;
}
