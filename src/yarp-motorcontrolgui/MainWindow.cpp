// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

//
// MainWindow implementation.
//
#include "MainWindow.h"

MainWindow::MainWindow(Glib::RefPtr<Gnome::Glade::Xml> xml) : refXml(xml) {
    mainWindow = 0;
    filechooserButton = 0;
    filechooserDialog = 0;
    filenameEntry = 0;
    canCombo = 0;
    connectButton = 0;
    stopButton = 0;

    mainWindow = (Gtk::Window *)getWidget("window_main");

    filechooserButton = (Gtk::Button *)getWidget("button_filechooser"); 
    filechooserDialog = (Gtk::FileChooserDialog *)getWidget("filechooserdialog_inifile");
    filenameEntry = (Gtk::Entry *)getWidget("entry_inifile"); 
    canCombo = (Gtk::ComboBox *)getWidget("combobox_can");
    connectButton = (Gtk::Button *)getWidget("button_connect");
    stopButton = (Gtk::Button *)getWidget("button_stop");

    axisCombo = (Gtk::ComboBox *)getWidget("combobox_axis");
    pEntry = (Gtk::Entry *)getWidget("entry_p");
    dEntry = (Gtk::Entry *)getWidget("entry_d");
    iEntry = (Gtk::Entry *)getWidget("entry_i");
    iLimitEntry = (Gtk::Entry *)getWidget("entry_ilim");
    shiftEntry = (Gtk::Entry *)getWidget("entry_shift");
    offsetEntry = (Gtk::Entry *)getWidget("entry_offset");
}

MainWindow::~MainWindow() {
    //delete mainWindow; memory is managed by gtkmm.
}

void MainWindow::initialize () {
    yarp.initialize();

    // initialize also the state of the GUI.
    canCombo->set_active(0);
    axisCombo->set_active(0);
    //axisCombo->set_sensitive(false);

    linkAll();
}

void MainWindow::linkAll () {
    filechooserButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onButtonFilechooserClicked));

    Gtk::ImageMenuItem *b = 
        (Gtk::ImageMenuItem *)getWidget("quit1");
    b->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onButtonQuitClicked));


    connectButton->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onButtonConnectClicked));
    // more signals.
}

void MainWindow::onButtonQuitClicked () {
    yarp.uninitialize();
    exit(0);
}

void MainWindow::onButtonFilechooserClicked () {
    filechooserDialog->set_transient_for(*mainWindow);
    int result = filechooserDialog->run();
    Glib::ustring filename;
    switch (result) {
        case Gtk::RESPONSE_OK:
            filename = filechooserDialog->get_filename();
            filenameEntry->set_text(Glib::filename_to_utf8(filename));
            break;
        default:
            // do nothing.
            break;
    }
    filechooserDialog->hide();
}

void MainWindow::onButtonConnectClicked() {
    int i = canCombo->get_active_row_number();

    if (i == -1) {
        std::cout << "You need to select the device type" << std::endl;
        return;
    }
    else if (i == 0)
        if (!yarp.connectDevice(false)) {
            std::cout << "Troubles connecting to the device" << std::endl;
            return;
        }
    else
        yarp.connectDevice(true);

    // activates the interface (set sensitive).
}
