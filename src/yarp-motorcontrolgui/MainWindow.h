// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

//
// MainWindow class definition.
//

#ifndef __mainwindow__
#define __mainwindow__

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <libglademm.h>
#include <gtkmm.h>

#include "Gui2Yarp.h"

/**
 * MainWindow, a helper class that wraps the access to the main window widget
 * and the contained widgets.
 */
class MainWindow {
public:
    explicit MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml);
    ~MainWindow();

    /**
     * getMainWindow returns a reference to the main window widget.
     */
    Gtk::Window& getMainWindow() const { return *mainWindow; }

    /**
     * initialize the GUI to a default state.
     */
    void initialize();

    /**
     * quit the application.
     */
    void onButtonQuitClicked();

    /**
     * open the file chooser dialog.
     */
    void onButtonFilechooserClicked();

    /**
     * connect to the device driver.
     */
    void onButtonConnectClicked();

    /**
     * close the connection with the device.
     */
    void onButtonStopClicked();

protected:
    /**
     * linkAll connects all signals to widgets.
     */
    void linkAll();

    /**
     * get a widget from the local xml object and throw an exception on error.
     * @param s is the name of the widget.
     * @return  a pointer to a widget object.
     */
    inline Gtk::Widget * getWidget(std::string s) {
        Gtk::Widget *p = 0;
        refXml->get_widget(s, p);
        if (!p) {
            std::cout << "Can't get " << s << " widget" << std::endl;
            throw std::runtime_error("The requested Widget doesn't exist"); 
        }
        return p;
    }

    /**
     * activate the interface.
     */
    void activateInterface();

    /**
     * deactivate the interface.
     */
    void deactivateInterface();

protected:
    // the interface with yarp driver code.
    Gui2Yarp yarp;

    Glib::RefPtr<Gnome::Glade::Xml> refXml;
    Gtk::Window *mainWindow;

    // driver connection frame.
    Gtk::Button *filechooserButton;
    Gtk::FileChooserDialog *filechooserDialog;
    Gtk::Entry *filenameEntry;
    Gtk::ComboBox *canCombo;
    Gtk::Button *connectButton;
    Gtk::Button *stopButton;

    // PID parameters.
    Gtk::ComboBox *axisCombo;
    Gtk::Entry *pEntry;
    Gtk::Entry *dEntry;
    Gtk::Entry *iEntry;
    Gtk::Entry *iLimitEntry;
    Gtk::Entry *shiftEntry;
    Gtk::Entry *offsetEntry;
    Gtk::Entry *pwmLimitEntry;
    Gtk::Button *updateButton;
};

#endif
