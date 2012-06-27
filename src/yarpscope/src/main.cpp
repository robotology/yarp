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

#include <gtkmm/main.h>
#include <glibmm/i18n.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    //resource finder
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
        std::cerr << "FATAL: Cannot connect to yarp network";
        return -1;
    }

    if(!Glib::thread_supported()) {
        Glib::thread_init();
    }

    Gtk::Main kit(argc, argv);
    Glib::set_application_name(_("gPortScope"));

    GPortScope::MainWindow window;
    Gtk::Main::run(window);
    return 0;
}
