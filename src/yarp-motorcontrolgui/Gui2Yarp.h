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

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/String.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp;

/**
 * Gui2Yarp class definition.  Interfaces the gtk/glade GUI components
 * to the Yarp management of the motor control device driver by implementing
 * callbacks.
 */
class Gui2Yarp {
protected:    
    // here's where the yarp data should go.    
    String initFilename;

public:
    /**
     * initialize the device driver config file location.
     * @param s the filename including the path.
     */
    void setInitFilename(String& s) { initFilename = s; }
};