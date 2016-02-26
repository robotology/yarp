/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_PARSENAME_INC
#define YARPDB_PARSENAME_INC

#include <yarp/os/ConstString.h>

class ParseName {
private:
    yarp::os::ConstString carrier;
    yarp::os::ConstString networkChoice;
    yarp::os::ConstString portName;
public:
    void apply(const yarp::os::ConstString& str);

    yarp::os::ConstString getPortName() {
        return portName;
    }

    yarp::os::ConstString getCarrier() {
        return carrier;
    }

    yarp::os::ConstString getNetworkChoice() {
        return networkChoice;
    }

    void resetCarrier() {
        carrier = "";
    }
};

#endif
