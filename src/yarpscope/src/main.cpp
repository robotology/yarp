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

void usage() {
    std::cout << "Usage: gPortScope [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << " --help             Print this help and exit." << std::endl;
    std::cout << " --verbose          Verbose mode." << std::endl;
    std::cout << " --interval=[int]   Initial refresh interval in milliseconds. (default 50ms)" << std::endl;
    std::cout << " --local=[string]   Base name to assign to the local port(s). (default" << std::endl;
    std::cout << "                    /gPortScope/vector:i)" << std::endl;
    std::cout << " --remote=[string]  Remote port(s) to connect to at startup (default none)." << std::endl;
    std::cout << " --rows=[uint]      Number of rows. (default 1)" << std::endl;
    std::cout << " --cols=[uint]      Number of columns. (default 1)" << std::endl;
    std::cout << " --dx=[uint]        Initial width of the window." << std::endl;
    std::cout << " --dy=[uint]        Initial height of the window." << std::endl;
    std::cout << " --x=[uint]         Initial X position of the window." << std::endl;
    std::cout << " --y=[uint]         Initial Y position of the window." << std::endl;
}

int main(int argc, char *argv[])
{
    // Setup resource finder
    yarp::os::ResourceFinder rf;
//    rf.setVerbose();
//    rf.setDefaultContext("gPortScope/conf");
//    rf.setDefaultConfigFile("gPortScope.ini");
    rf.configure("ICUB_ROOT", argc, argv);

    rf.setDefault("local", "/gPortScope/vector:i");

    //Yarp network initialization
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fatal() << "Cannot connect to yarp network";
    }

    // Read command line options
    yarp::os::Property options;
    options.fromString(rf.toString());

//    if (options.check("verbose")) {
        GPortScope::setDebug(true);
//    }

    if (options.check("help")) {
        usage();
        exit(0);
    }

    // interval
    unsigned int interval = 50;
    if (options.check("interval")) {
        interval = options.find("interval").asInt();
    }
    debug() << "Using" << interval << "ms interval.";

    // local
    yarp::os::Value &localValue = options.find("local");
    if (localValue.isNull()) {
        std::cerr << "ERROR: local should not be empty." << std::endl << std::endl;
        usage();
        exit(1);
    }
    Glib::ustring local = localValue.toString().c_str();
    debug() << "Using" << local << "as base name for local port(s).";

    // remote
    Glib::ustring remote;
    yarp::os::Value &remoteValue = options.find("remote");
    if (remoteValue.isNull()) {
        debug() << "Missing --remote option. Will wait for the connection...";
    } else {
        remote = remoteValue.toString().c_str();
        debug() << "Using remote port(s)" << remote;
    }

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
        std::cerr << "ERROR: rows and cols should be positive integers." << std::endl << std::endl;
        usage();
        exit(1);
    }
    debug() << "Using" << rows << "rows and" << cols << "columns.";

    // Init gtkmm and gtkdataboxmm
    if(!Glib::thread_supported()) {
        Glib::thread_init();
    }
    Gtk::Main kit(argc, argv);
    GtkDataboxMM::init();
    Glib::set_application_name(_("gPortScope"));

    // Create main window and update geometry and title
    GPortScope::MainWindow mainWindow(interval, local, remote, rows, cols);

    if (rf.find("dx").isInt() && rf.find("dy").isInt()) {
        mainWindow.resize(rf.find("dx").asInt(), rf.find("dy").asInt());
    }
    if (rf.find("x").isInt() && rf.find("y").isInt()) {
        mainWindow.move(rf.find("x").asInt(), rf.find("y").asInt());
    }
    if (rf.find("title").isString()) {
        mainWindow.set_title(rf.find("title").asString().c_str());
    }


    Gtk::Main::run(mainWindow);
    return 0;
}
