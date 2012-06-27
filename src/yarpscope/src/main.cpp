/*
 *  This file is part of gPortScope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <iostream>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#include <gtkmm/main.h>
#include <glibmm/i18n.h>

#include <gtkdataboxmm/init.h>

#include "MainWindow.h"
#include "Debug.h"
#include "XmlLoader.h"
#include "PlotManager.h"
#include "PortReader.h"

void usage() {
    std::cout << "Usage: gPortScope [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << " --help             Print this help and exit." << std::endl;
    std::cout << std::endl;
    std::cout << " --title [string]   Title of the window (default \"gPortScope\")" << std::endl;
    std::cout << " --x [uint]         Initial X position of the window." << std::endl;
    std::cout << " --y [uint]         Initial Y position of the window." << std::endl;
    std::cout << " --dx [uint]        Initial width of the window." << std::endl;
    std::cout << " --dy [uint]        Initial height of the window." << std::endl;
    std::cout << std::endl;
    std::cout << " --interval [int]   Initial refresh interval in milliseconds. (default = 50ms)" << std::endl;
    std::cout << std::endl;
    std::cout << "XML MODE:" << std::endl;
    std::cout << " --xml [path]       Path to the xml with the description of the scene (all the" << std::endl;
    std::cout << "                    \"command line mode\" options are discarded)." << std::endl;
    std::cout << std::endl;
    std::cout << "COMMAND LINE MODE:" << std::endl;
    std::cout << " --remote [...]     Remote port(s) to use. It can be:" << std::endl;
    std::cout << "                    * [string] (single plot connected to a single remote port)" << std::endl;
    std::cout << "                    * [array of strings] (single plot connected to several" << std::endl;
    std::cout << "                      ports)" << std::endl;
    std::cout << "                    * [array of arrays of string] (multiple plots each connected" << std::endl;
    std::cout << "                      to multiple ports)" << std::endl;
    std::cout << " --index [...]      Index(es) of the vector to plot. Depending on the value of the \"--remote\"" << std::endl;
    std::cout << "                    option it must be:" << std::endl;
    std::cout << "                    * [array of uint]" << std::endl;
    std::cout << "                    * [array of arrays of uint]" << std::endl;
    std::cout << "                    * [array of arrays of arrays of uint]" << std::endl;
    std::cout << " --rows [uint]      Number of rows. (default = number of remotes)" << std::endl;
    std::cout << " --cols [uint]      Number of columns. (default = 1)" << std::endl;
    std::cout << std::endl;
    std::cout << "LEGACY OPTIONS (deprecated and unused):" << std::endl;
    std::cout << " --local [string]   Use YARP_PORT_PREFIX environment variable to modify default value." << std::endl;
}


int main(int argc, char *argv[])
{
    // Setup resource finder
    yarp::os::ResourceFinder rf;
//    rf.setVerbose();
//    rf.setDefaultContext("gPortScope/conf");
//    rf.setDefaultConfigFile("gPortScope.ini");
    rf.configure("ICUB_ROOT", argc, argv); // FIXME Use another policy

    //Yarp network initialization
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fatal() << "Cannot connect to yarp network";
    }

    // Read command line options
    yarp::os::Property options;
    options.fromString(rf.toString());

    if (options.check("help")) {
        usage();
        exit(0);
    }

    // Init gtkmm and gtkdataboxmm
    if(!Glib::thread_supported()) {
        Glib::thread_init();
    }
    Gtk::Main kit(argc, argv);
    GDatabox::init();
    Glib::set_application_name(_("gPortScope"));


    // Create main window
    GPortScope::MainWindow mainWindow;


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


    if (options.check("xml")) {
// XML Mode Options
        const yarp::os::Value &xmlValue = options.find("xml");
        debug() << "Loading file" << xmlValue.toString();
        GPortScope::XmlLoader xmlLoader(xmlValue.toString().c_str());
    } else {
// Command Line Mode Options
        // TODO Make a class
        // rows and cols
        unsigned int rows = 1;
        unsigned int cols = 1;
        if (options.check("rows")) {
            rows = options.find("rows").asInt();
        }
        if (options.check("cols")) {
            cols = options.find("cols").asInt();
        }
        if (rows <= 0 || cols <= 0) {
            error() << "--rows and --cols should be positive integers.\n";
            usage();
            exit(1);
        }
        debug() << "Using" << rows << "rows and" << cols << "columns.";

        // remote
        const yarp::os::Value &remoteValue = options.find("remote");
        if (remoteValue.isNull()) {
            debug() << "Missing --remote option. Will wait for the connection...";
        } else {
            debug() << "Using remote port(s)" << remoteValue.toString();
        }

        // index
        yarp::os::Value &indexValue = options.find("index");
        if (indexValue.isNull()) {
            debug() << "No index parameter. Plotting index \"0\"";
            indexValue.fromString("0");
        }

        bool remoteValueOk = true;
        bool indexValueOk = true;

        for (unsigned int i = 0; i < rows * cols; i++) {
            if (!remoteValueOk || !indexValueOk) {
                break;
            }

            std::list<Glib::ustring> remotePortNames;

            //local/remote port connection
            if (remoteValue.isString()) {
                remotePortNames.push_back(remoteValue.toString().c_str());
            } else if (remoteValue.isList()) {
                yarp::os::Bottle *remoteList = remoteValue.asList();
                unsigned int listSize = remoteList->size();
                if (remoteList->get(0).isString() && listSize == 1) {
                    remotePortNames.push_back(remoteList->get(0).toString().c_str());
                } else if (remoteList->get(i).isString() && listSize == rows * cols) {
                    remotePortNames.push_back(remoteList->get(i).toString().c_str());
                } else if (remoteList->get(i).isList() && listSize == rows * cols) {
                    yarp::os::Bottle *portRemoteList = remoteList->get(i).asList();
                    for (int j = 0; j < portRemoteList->size(); j++) {
                        if (!portRemoteList->get(j).isString()) {
                            remoteValueOk = false;
                            break;
                        }
                        remotePortNames.push_back(portRemoteList->get(j).toString().c_str());
                    }
                } else {
                    remoteValueOk = false;
                    break;
                }
            } else if (!remoteValue.isNull()) {
                remoteValueOk = false;
                break;
            }
        }

        if (!remoteValueOk) {
            error() << "--remote argument should be a string or a list of rows * cols strings or lists of strings\n";
            usage();
            exit(1);
        }

        if (!indexValueOk) {
            error() << "--index argument should be a integer or a list of rows * cols integer or lists of integers or lists of integers\n";
            usage();
            exit(1);
        }
    }

// Deprecated options
    // local
    if (options.check("local")) {
        warning() << "--local option is deprecated. gPortScope now uses \"${YARP_PORT_PREFIX}/gPortScope/${REMOTE_PORT_NAME}\"";
    }


    Gtk::Main::run(mainWindow);
    return 0;
}
