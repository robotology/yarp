/*
 * Copyright (C) 2006  Robotcub Consortium
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Francesco Nori, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#include <gtkmm.h>

//#include <glibmm/i18n.h>
#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#include <gtkdataboxmm/init.h>

#include "MainWindow.h"
#include "Debug.h"
#include "XmlLoader.h"
#include "PlotManager.h"
#include "PortReader.h"
#include "SimpleLoader.h"

void usage() {
    std::cout << "Usage: yarpscope [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << " --help                 Print this help and exit." << std::endl;
    std::cout << std::endl;
    std::cout << " --title [string]       Title of the window (default \"Yarp Port Scope\")" << std::endl;
    std::cout << " --x [uint]             Initial X position of the window." << std::endl;
    std::cout << " --y [uint]             Initial Y position of the window." << std::endl;
    std::cout << " --dx [uint]            Initial width of the window." << std::endl;
    std::cout << " --dy [uint]            Initial height of the window." << std::endl;
    std::cout << std::endl;
    std::cout << " --interval [int]       Initial refresh interval in milliseconds. (default = 50ms)" << std::endl;
    std::cout << std::endl;
    std::cout << "XML MODE:" << std::endl;
    std::cout << " --xml [path]           Path to the xml with the description of the scene (all the" << std::endl;
    std::cout << "                        \"simple mode\" options are discarded)." << std::endl;
    std::cout << std::endl;
    std::cout << "SIMPLE MODE (single remote):" << std::endl;
    std::cout << " --remote [string]      Remote port to connect to." << std::endl;
    std::cout << " --carrier [string]     YARP Carrier used for connections (default \"mcast\")" << std::endl;
//     std::cout << " --no-persistent        Do not make persistent connections" << std::endl;
    std::cout << " --index [...]          Index(es) of the vector to plot." << std::endl;
    std::cout << "                        It can be an [uint] or an array of [uint]s" << std::endl;
    std::cout << " --plot_title [string]  Plot title (default = remote)" << std::endl;
    std::cout << " --min [float]          Minimum value for the X axis (default -100)" << std::endl;
    std::cout << " --max [float]          Maximum value for the X axis (default 100)" << std::endl;
    std::cout << " --size [uint]          Plot size (Number of samples to plot) (default 201)" << std::endl;
    std::cout << " --bgcolor [string]     Background color." << std::endl;
//    std::cout << " --autorescale          Rescale plot automatically." << std::endl;
//    std::cout << " --realtime             Use real time mode." << std::endl;
//    std::cout << " --triggermode          Use trigger mode." << std::endl;
//    std::cout << " --graph_title [...]    Graph title(s) (used in legend)." << std::endl;
//    std::cout << "                        Depending on index it mast be a [string] or an array of [string]s." << std::endl;
    std::cout << " --color [...]          Graph color(s)." << std::endl;
    std::cout << "                        Depending on index it must be a [string] or an array of [string]s." << std::endl;
    std::cout << " --type [...]           Graph type(s). Accepted values are \"points\", \"lines\" and \"bars\" (default = \"lines\")" << std::endl;
    std::cout << "                        Depending on index it must be a [string] or an array of [string]s." << std::endl;
    std::cout << " --graph_size [...]     Graph size(s) (thickness of the points) (default = 1)" << std::endl;
    std::cout << "                        Depending on index it must be a [uint] or an array of [uint]s." << std::endl;
    std::cout << std::endl;
    std::cout << "LEGACY OPTIONS (deprecated and unused):" << std::endl;
    std::cout << " --local [string]       Use YARP_PORT_PREFIX environment variable to modify default value." << std::endl;
    std::cout << " --rows [uint]          Only one plot is supported from command line. Use XML mode instead." << std::endl;
    std::cout << " --cols [uint]          Only one plot is supported from command line. Use XML mode instead." << std::endl;
}


int main(int argc, char *argv[])
{
    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultContext("yarpscope/conf");
    rf.setDefaultConfigFile("yarpscope.ini");
    rf.configure(argc, argv);

    // Read command line options
    yarp::os::Property options;
    options.fromString(rf.toString());

    if (options.check("help")) {
        usage();
        exit(0);
    }

    //Yarp network initialization
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fatal() << "Cannot connect to yarp network";
    }

    // Init gtkmm and gtkdataboxmm
    Gtk::Main kit(argc, argv);
    GDatabox::init();
    Glib::set_application_name(_("Yarp Port Scope"));


    // Create main window
    YarpScope::MainWindow mainWindow;


// Generic options
    // title
    if (rf.find("title").isString()) {
        mainWindow.set_title(rf.find("title").asString().c_str());
    }
    // position
    if (rf.find("x").isInt() && rf.find("y").isInt()) {
        mainWindow.move(rf.find("x").asInt(), rf.find("y").asInt());
    }
    // size
    if (rf.find("dx").isInt() && rf.find("dy").isInt()) {
        mainWindow.resize(rf.find("dx").asInt(), rf.find("dy").asInt());
    }
    // interval
    if (options.check("interval")) {
        mainWindow.setInterval(options.find("interval").asInt());
    }

    bool ok;
    if (options.check("xml")) {
// XML Mode Options
        const yarp::os::Value &xmlValue = options.find("xml");
        const yarp::os::ConstString &filename = rf.findFile(xmlValue.toString().c_str());
        debug() << "Loading file" << filename;
        YarpScope::XmlLoader xmlLoader(filename.c_str());
    } else {
// Command Line Mode Options
        debug() << "Loading from command line";
        YarpScope::SimpleLoader simpleLoader(options, &ok);
        if (!ok) {
            usage();
            exit(1);
        }
    }

// Deprecated options
    // local
    if (options.check("local")) {
        warning() << "--local option is deprecated. YarpScope now uses \"${YARP_PORT_PREFIX}/YarpScope/${REMOTE_PORT_NAME}\"";
    }

    // rows
    if (options.check("rows")) {
        warning() << "--rows option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }

    // cols
    if (options.check("cols")) {
        warning() << "--cols option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }

    Gtk::Main::run(mainWindow);
    return 0;
}
