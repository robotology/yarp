// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

//
// MainWindow class definition.
//

#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

/**
 * MainWindow, a helper class that wraps the access to the main window widget.
 */
class MainWindow {
public:
    explicit MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml);
    ~MainWindow();

    Gtk::Window& getWindow() const { return *mainWindow; }

protected:
    Gtk::Window *mainWindow;
};

