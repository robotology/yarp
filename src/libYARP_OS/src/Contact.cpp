// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Contact.h>
#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Value.h>

using namespace yarp::os::impl;
using namespace yarp::os;


// implementation is an Address
#define HELPER(x) (*((Address*)(x)))

Contact::Contact() {
    implementation = new Address();
    YARP_ASSERT(implementation!=NULL);
}

Contact::~Contact() {
    if (implementation!=NULL) {
        delete (Address*)implementation;
        implementation = NULL;
    }
}


Contact::Contact(const Contact& alt) {
    implementation = new Address();
    YARP_ASSERT(implementation!=NULL);
    HELPER(implementation) = HELPER(alt.implementation);
}


const Contact& Contact::operator = (const Contact& alt) {
    HELPER(implementation) = HELPER(alt.implementation);
    return (*this);
}


Contact Contact::empty() {
    return Contact();
}


Contact Contact::invalid() {
    return Contact().addSocket("","",-1);
}


Contact Contact::byName(const char *name) {
    Contact result;
    HELPER(result.implementation) = Address().addRegName(name);
    return result;
}


Contact Contact::byCarrier(const char *carrier) {
    Contact result;
    HELPER(result.implementation) = Address("",0,carrier);
    return result;
}

Contact Contact::addCarrier(const char *carrier) const {
    Contact result;
    HELPER(result.implementation) = HELPER(implementation).addCarrier(carrier);
    return result;
}


Contact Contact::bySocket(const char *carrier, 
                          const char *machineName,
                          int portNumber) {
    Contact result;
    HELPER(result.implementation) = Address(machineName,portNumber,carrier);
    return result;
}


Contact Contact::addSocket(const char *carrier, 
                           const char *machineName,
                           int portNumber) const {
    Contact result;
    HELPER(result.implementation) = HELPER(implementation).addSocket(carrier,
                                                                     machineName,
                                                                     portNumber);
    return result;
}


ConstString Contact::getName() const {
    return HELPER(implementation).getRegName().c_str();
}


ConstString Contact::getHost() const {
    return HELPER(implementation).getName().c_str();
}


ConstString Contact::getCarrier() const {
    return HELPER(implementation).getCarrierName().c_str();
}


int Contact::getPort() const {
    return HELPER(implementation).getPort();
}


ConstString Contact::toString() const {
    Address& addr = HELPER(implementation);
    String result = addr.getRegName();
    if (addr.getCarrierName()!="") {
        result = addr.getCarrierName() + ":/" + result;
    }
    return ConstString(result.c_str());
}


bool Contact::isValid() const {
    Address& addr = HELPER(implementation);
    return addr.getPort()>=0;
}


Contact Contact::byConfig(Searchable& config) {
    Contact result;
    Address& addr = HELPER(result.implementation);
    int port = config.check("port_number",Value(-1)).asInt();
    String name = config.check("ip",Value("")).asString().c_str();
    String regName = config.check("name",Value("")).asString().c_str();
    String carrier = config.check("carrier",Value("tcp")).asString().c_str();
    addr = Address(name,port,carrier,regName);
    return result;
}


