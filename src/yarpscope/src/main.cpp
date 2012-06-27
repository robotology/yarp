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


int main(int argc, char *argv[])
{
    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultContext("gPortScope/conf");
    rf.setDefaultConfigFile("gPortScope.ini");
    rf.configure("ICUB_ROOT", argc, argv);

    rf.setDefault("local", "/gPortScope/vector:i");
    rf.setDefault("remote", "");
    rf.setDefault("robot", "icub");
    rf.setDefault("part", "head");

    //Yarp network initialization
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fatal() << "Cannot connect to yarp network";
    }

    // Read command line options
    yarp::os::Property options;
    options.fromString(rf.toString());

    yarp::os::Value &robot    = options.find("robot");
    yarp::os::Value &part     = options.find("part");
    yarp::os::Value &rows     = options.find("rows");
    yarp::os::Value &cols     = options.find("cols");
    yarp::os::Value &interval = options.find("interval");


//    if (options.check("verbose")) {
        GPortScope::setDebug(true);
//    }

    if (!options.check("robot")) {
        debug() << "Missing --robot option. Using icub.";
    } else {
        debug() << "Using" << robot.toString() << "robot.";
    }

    if (!options.check("part")) {
        debug() << "Missing --part option. Using head.";
    } else {
        debug() << "Using" << part.toString() << "part.";
    }

    if (!options.check("local")) {
        debug() << "Missing --name option. Using /portScope/vector:i.";
    }

    if (!options.check("remote")) {
        debug() << "Missing --remote option. Will wait for the connection...";
    }

    if (!options.check("rows")) {
        debug() << "Missing --rows option. Using 1.";
    } else {
        debug() << "Using" << rows.asInt() << "rows.";
    }

    if (!options.check("cols")) {
        debug() << "Missing --cols option. Using 1.";
    } else {
        debug() << "Using" << cols.asInt() << "columns.";
    }

    if (!options.check("interval")) {
        debug() << "Missing --interval option. Using 50 ms.";
    } else {
        debug() << "Using" << interval.asInt() << "ms interval.";
    }

    // Init gtkmm and gtkdataboxmm
    if(!Glib::thread_supported()) {
        Glib::thread_init();
    }
    Gtk::Main kit(argc, argv);
    GtkDataboxMM::init();
    Glib::set_application_name(_("gPortScope"));

    // Create main window and update geometry and title
    GPortScope::MainWindow mainWindow;

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
