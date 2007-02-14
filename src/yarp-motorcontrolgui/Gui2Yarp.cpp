// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * This is the motorcontrol GUI, under development.
 *
 */

#include "Gui2Yarp.h"


void Gui2Yarp::linkAll (Glib::RefPtr<Gnome::Glade::Xml> refXml) {

    Gtk::Button* but = 0;
    refXml->get_widget("button_quit", but);
    if (!but)
        throw std::runtime_error("Couldn't find the quit button as requested");
    but->signal_clicked().connect(sigc::mem_fun(*this, &Gui2Yarp::onButtonQuitClicked));
    
}

void Gui2Yarp::onButtonQuitClicked () {
    std::cout << "quit clicked!" << std::endl;
    exit(0);
}
