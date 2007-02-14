// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

/**
 * Gui2Yarp class definition.  Interfaces the gtk/glade GUI components
 * to the Yarp management of the motor control device driver by implementing
 * callbacks.
 */
class Gui2Yarp {
protected:

public:
    /**
     * linkAll connects all signals to widgets.
     * @param refXml is a reference count pointer to the xml glade representation.
     */
    void linkAll(Glib::RefPtr<Gnome::Glade::Xml> refXml);

    /**
     * quit the application.
     */
    void onButtonQuitClicked();

};