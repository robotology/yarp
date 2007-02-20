// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * This is the motorcontrol GUI, under development.
 *
 */

#include "Gui2Yarp.h"

bool Gui2Yarp::initialize() {
    Network::init();
	Time::turboBoost();      

    connected = false;
    positionControl = 0;
    amplifierControl = 0;

    return true;
}

bool Gui2Yarp::uninitialize() {
    Network::fini();
    connected = false;
    return true;
}

bool Gui2Yarp::connectDevice(bool local) {
    std::cout << "Instantiating the device driver" << std::endl;

    options.fromConfigFile(initFilename.c_str()); // no return value?
    if (!options.findGroup("DEVICE").check("device")) {
        std::cout << "It seems that the config file is missing the device specification" << std::endl;
        throw std::runtime_error("Bad config file, please see the documentation");
    }

    // I'm not dealing with the calibrator for now.
    std::cout << options.toString().c_str() << std::endl;

    if (local) {
        driver.open(options);
        if (!driver.isValid()) {
            std::cout << "The driver filed to load" << std::endl;
            connected = false;
            return false;
        }
        // acquire the interfaces.
        connected = true;
        connected &= driver.view(positionControl);
        connected &= driver.view(amplifierControl);
        if (!connected) {
            driver.close();
            positionControl = 0;
            amplifierControl = 0;
            return false;
        }
        return true;
    }
    else {
        std::cout << "Remote device driver connection not yet implemented" << std::endl;
        connected = false;
        return false;
    }

    return true;
}


