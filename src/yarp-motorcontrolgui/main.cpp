// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * This is the motorcontrol GUI, under development.
 *
 */

#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

#include "MainWindow.h"
#include "Gui2Yarp.h"

//
//
//
int main(int argc, char *argv[])
{
    try {
        Gtk::Main kit(argc, argv);
        Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("yarpmotorcontrol.glade");        
        MainWindow mainw(refXml);
        
        Gui2Yarp yarp;
        yarp.linkAll(refXml);

        kit.run(mainw.getWindow());
        return 0;
    }
    catch(...) {
        return 1;
    }
}

