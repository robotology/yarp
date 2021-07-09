/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ModifyingCarrier.h>

using namespace yarp::os;

bool yarp::os::ModifyingCarrier::checkHeader(const yarp::os::Bytes& header)
{
    YARP_UNUSED(header);
    return false;
}


void yarp::os::ModifyingCarrier::getHeader(yarp::os::Bytes& header) const
{
    if (header.length() == 8) {
        std::string target = "ohbehave";
        for (int i = 0; i < 8; i++) {
            header.get()[i] = target[i];
        }
    }
}

bool yarp::os::ModifyingCarrier::respondToHeader(yarp::os::ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return false;
}

bool yarp::os::ModifyingCarrier::modifiesIncomingData() const
{
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesOutgoingData() const
{
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesReply() const
{
    return true;
}


void yarp::os::ModifyingCarrier::setCarrierParams(const yarp::os::Property& params)
{
    YARP_UNUSED(params);
}

void yarp::os::ModifyingCarrier::getCarrierParams(yarp::os::Property& params) const
{
    YARP_UNUSED(params);
}

bool yarp::os::ModifyingCarrier::configureFromProperty(yarp::os::Property& prop)
{
    YARP_UNUSED(prop);
    return false;
}
