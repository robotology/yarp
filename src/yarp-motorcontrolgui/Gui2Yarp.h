// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __gui2yarp__
#define __gui2yarp__

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
    String initFilename;
    Property options;
    PolyDriver driver;

    // interfaces required.
    IPositionControl *positionControl;
    IVelocityControl *velocityControl;
    IAmplifierControl *amplifierControl;
    IEncoders *encoders;
    IPidControl *pid;
    IControlLimits *limits;

    bool connected;

public:
    /**
     * default constructor.
     */
    Gui2Yarp() {
        // initFilename = "";
    }

    /**
     * initialize the interface and network wrappers.
     * @return true is successful, false otherwise.
     */
    bool initialize();

    /**
     * shut down the connection to yarp stuff.
     * @return true is successful, false otherwise.
     */
    bool uninitialize();

    /**
     * initialize the device driver config file location.
     * @param s the filename including the path.
     */
    void setInitFilename(String& s) { initFilename = s; }

    /**
     * return the connection status as a bool.
     * @return the connection status.
     */
    bool isConnected() const { return connected; }

    /**
     * connect to the motor control device driver.
     * @param local, whethet the driver is local to the machine.
     * @return true if successful, false otherwise.
     */
    bool connectDevice(bool local = true);

    /**
     * disconnect the device driver.
     * @return true if successful, false otherwise.
     */
    bool disconnectDevice();

    /**
     * obtain the number of controlled axes.
     * @return the number of conrolled axes or 0 in case of failure.
     */
    int getAxes() const;

    /**
     * get the pid values from the control card.
     * @param j is the axis number.
     * @param v is a vector of double of length equal to 7.
     * @return true if successful, false otherwise.
     */
    bool getPid(int j, double* v) const;
};

#endif
