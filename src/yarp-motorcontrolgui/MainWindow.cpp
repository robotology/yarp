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

MainWindow::MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml) : mainWindow(0) {
    refXml->get_widget("window_main", mainWindow);
    if (!mainWindow)
        throw std::runtime_error("Couldn't find window_main");
}

MainWindow::~MainWindow() {
    //delete mainWindow; memory is managed by gtkmm.
}


