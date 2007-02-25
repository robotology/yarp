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
    velocityControl = 0;
    amplifierControl = 0;
    encoders = 0;
    pid = 0;
    limits = 0;
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
    if (!options.findGroup("DEVICE").check("Device")) {
        std::cout << "It seems that the config file is missing the device specification" << std::endl;
        throw std::runtime_error("Bad config file, please see the documentation");
    }

    // I'm not dealing with the calibrator for now.
    //std::cout << options.toString().c_str() << std::endl;

    if (local) {
        Value &device=options.findGroup("DEVICE").find("Subdevice");
        options.put("device", device);
        
        driver.open(options);
        if (!driver.isValid()) {
            std::cout << "The driver filed to load" << std::endl;
            connected = false;
            return false;
            //connected = true;
            //return true;
        }

        // acquire the interfaces.
        connected = driver.view(positionControl);
        connected &= driver.view(velocityControl);
        connected &= driver.view(amplifierControl);
        connected &= driver.view(encoders);
        connected &= driver.view(pid);
        connected &= driver.view(limits);
        if (!connected) {
            driver.close();
            positionControl = 0;
            velocityControl = 0;
            amplifierControl = 0;
            encoders = 0;
            pid = 0;
            limits = 0;
            return false;
        }
        return true;
    }
    else {
        // need device & subdevice here.
        std::cout << "Remote device driver connection not yet implemented" << std::endl;
        connected = false;
        return false;
    }

    return true;
}

bool Gui2Yarp::disconnectDevice() {
    if (driver.isValid())
        return driver.close();
    connected = false;
    return true;
}

int Gui2Yarp::getAxes() const {
    int ax = 0;
    if (encoders->getAxes(&ax))
        return ax;
    return 0;
}

bool Gui2Yarp::getPid(int j, double* v) const {
    Pid pd;
    bool result = pid->getPid(j, &pd);
    if (result && v) {
        v[0] = pd.kp;
        v[1] = pd.kd;
        v[2] = pd.ki;
        v[3] = pd.max_int;
        v[4] = pd.max_output;
        v[5] = pd.offset;
        v[6] = pd.scale;
        return true;
    }

    ACE_OS::memset(v, 0, 7);
    return false;
}

