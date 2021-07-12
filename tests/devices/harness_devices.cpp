/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "YarpBuildLocation.h"

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>
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

namespace {

static yarp::os::Network* net = nullptr;
static bool verbose = false;

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

static void init_Network()
{
    net = new yarp::os::Network;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    if (verbose) {
        yarp::os::NetworkBase::setVerbosity(1);
    }
YARP_WARNING_POP
#endif
}

static void fini_Network()
{
    delete net;
    net = nullptr;
}

static void setup_Environment()
{
    // To make sure that the dev test are able to find all and only the devices
    // compiled by YARP, including the ones compiled as dynamic plugins,
    // YARP_DATA_DIRS is set to the build directory + the TEST_DATA_DIR
    // and YARP_DATA_HOME is set to a non existent directory
    std::string yarp_data_dirs =
            CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "share" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp" +
            std::string{yarp::conf::environment::path_separator} +
            TEST_DATA_DIR;
    yarp::conf::environment::set_string("YARP_DATA_DIRS", yarp_data_dirs);

    std::string yarp_data_home =
            CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "home" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "user" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            ".local" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_DATA_HOME", yarp_data_home);

    // To ensure that this will behave in the same way if YARP is configured on
    // the user's system and on the build machines, YARP_CONFIG_DIRS and
    // YARP_CONFIG_HOME are set to a non existent directory
    std::string yarp_config_dirs = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "etc" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_CONFIG_DIRS", yarp_config_dirs);

    std::string yarp_config_home = CMAKE_BINARY_DIR +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "home" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "user" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            ".config" +
            std::string{yarp::conf::filesystem::preferred_separator} +
            "yarp";
    yarp::conf::environment::set_string("YARP_CONFIG_HOME", yarp_config_home);

    if (verbose) {
        printf("YARP_DATA_DIRS=\"%s\"\n", yarp::conf::environment::get_string("YARP_DATA_DIRS").c_str());
        printf("YARP_DATA_HOME=\"%s\"\n", yarp::conf::environment::get_string("YARP_DATA_HOME").c_str());
        printf("YARP_CONFIG_DIRS=\"%s\"\n", yarp::conf::environment::get_string("YARP_CONFIG_DIRS").c_str());
        printf("YARP_CONFIG_HOME=\"%s\"\n", yarp::conf::environment::get_string("YARP_CONFIG_HOME").c_str());
    }
}

} // namespace

int main(int argc, char *argv[])
{
    Property p;
    p.fromCommand(argc,argv);
    verbose = p.check("verbose") && p.check("verbose", Value(true)).asBool();

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

    auto start = fileName.rfind('/');
    if (start == std::string::npos) {
        start = fileName.rfind('\\');
    }
    if (start == std::string::npos) {
        start = 0;
    } else {
        ++start;
    }
    auto len = fileName.find('.', start);
    if (len != std::string::npos) {
        len -= start;
    }
    const std::string exampleName = fileName.substr(start, len);
    const std::string shortFileName = fileName.substr(start);

    setup_Environment();
    init_Network();
    Network::setLocalMode(true);

    PolyDriver dd;
    yInfo("Opening PolyDriver for device %s", deviceName.c_str());
    if (!dd.open(p)) {
        yError("PolyDriver failed to opened.");
        return 254;
    }

    yInfo("PolyDriver opened.");

    std::string wrapperName = "";
    std::string codeName = "";

    DriverCreator *creator = Drivers::factory().find(deviceName.c_str());


    if (creator!=nullptr) {
        wrapperName = creator->getWrapper();
        codeName = creator->getCode();
    }
    yDebug("wrapperName = %s, codeName = %s", wrapperName.c_str(), codeName.c_str());

    FILE *fout = fopen(dest.c_str(), "w");
    if (fout == nullptr) {
        yError("Problem writing to %s", dest.c_str());
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

    yInfo("Starting 1 second delay");
    yarp::os::SystemClock::delaySystem(1);

    yInfo("Closing PolyDriver");
    dd.close();
    yInfo("PolyDriver closed.");

    fini_Network();

    return 0;
}
