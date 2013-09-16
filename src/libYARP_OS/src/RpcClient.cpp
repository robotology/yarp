// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcClient.h>


yarp::os::RpcClient::RpcClient() :
        Contactable() {
    // should configure port object to let it know it will be used
    // as an RPC client - TODO.
}

yarp::os::RpcClient::~RpcClient() {
    port.close();
}

bool yarp::os::RpcClient::open(const ConstString& name) {
    port.setInputMode(false);
    port.setOutputMode(true);
    port.setRpcMode(true);
    return port.open(name);
}

bool yarp::os::RpcClient::open(const Contact& contact, bool registerName) {
    return port.open(contact,registerName);
}

bool yarp::os::RpcClient::addOutput(const ConstString& name) {
    return port.addOutput(name);
}

bool yarp::os::RpcClient::addOutput(const ConstString& name, 
                                    const ConstString& carrier) {
    return port.addOutput(name,carrier);
}

bool yarp::os::RpcClient::addOutput(const Contact& contact){
    return port.addOutput(contact);
}

void yarp::os::RpcClient::close() {
    port.close();
}

void yarp::os::RpcClient::interrupt() {
    port.interrupt();
}

void yarp::os::RpcClient::resume() {
    port.resume();
}

yarp::os::Contact yarp::os::RpcClient::where() const {
    return port.where();
}

yarp::os::ConstString yarp::os::RpcClient::getName() const {
    return where().getName();
}

bool yarp::os::RpcClient::write(PortWriter& writer, PortReader& reader) const {
    return port.write(writer,reader);
}

bool yarp::os::RpcClient::setEnvelope(PortWriter& envelope) {
    return port.setEnvelope(envelope);
}

bool yarp::os::RpcClient::getEnvelope(PortReader& envelope) {
    return port.getEnvelope(envelope);
}

int yarp::os::RpcClient::getInputCount() {
    return port.getInputCount();
}

int yarp::os::RpcClient::getOutputCount() {
    return port.getOutputCount();
}

void yarp::os::RpcClient::getReport(PortReport& reporter) {
    port.getReport(reporter);
}

void yarp::os::RpcClient::setReporter(PortReport& reporter) {
    port.setReporter(reporter);
}

bool yarp::os::RpcClient::isWriting() {
    return port.isWriting();
}

void yarp::os::RpcClient::setReader(PortReader& reader) {
    port.setReader(reader);
}


yarp::os::Type yarp::os::RpcClient::getType() {
    return port.getType();
}


yarp::os::Property *yarp::os::RpcClient::acquireProperties(bool readOnly) {
    return port.acquireProperties(readOnly);
}

void yarp::os::RpcClient::releaseProperties(Property *prop) {
    port.releaseProperties(prop);
}
