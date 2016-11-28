/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_SERVERSQL_IMPL_PARSENAME_H
#define YARP_SERVERSQL_IMPL_PARSENAME_H

#include <yarp/os/ConstString.h>


namespace yarp {
namespace serversql {
namespace impl {

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

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_PARSENAME_H
