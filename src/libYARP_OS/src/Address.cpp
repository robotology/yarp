// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/Address.h>

using namespace yarp;
using namespace yarp::os;

Contact Address::toContact() const {
    return Contact::byName
        (getRegName().c_str()).addSocket(getCarrierName().c_str(),
                                         getName().c_str(),
                                         getPort());
}

Address Address::fromContact(const Contact& contact) {
    return Address(contact.getHost().c_str(),
                   contact.getPort(),
                   contact.getCarrier().c_str(),
                   contact.getName().c_str());
}


