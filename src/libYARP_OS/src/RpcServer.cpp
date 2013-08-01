// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcServer.h>


yarp::os::RpcServer::RpcServer() :
        Contactable() {
    // should configure port object to let it know it will be used
    // as an RPC server - TODO.
}

yarp::os::RpcServer::~RpcServer() {
    port.close();
}

bool yarp::os::RpcServer::open(const ConstString& name) {
    port.setInputMode(true);
    port.setOutputMode(false);
    port.setRpcMode(true);
    return port.open(name);
}

bool yarp::os::RpcServer::open(const Contact& contact, bool registerName) {
    return port.open(contact,registerName);
}

bool yarp::os::RpcServer::addOutput(const ConstString& name) {
    return false;
}

bool yarp::os::RpcServer::addOutput(const ConstString& name, 
                                    const ConstString& carrier) {
    return false;
}

bool yarp::os::RpcServer::addOutput(const Contact& contact){
    return false;
}

void yarp::os::RpcServer::close() {
    port.close();
}

void yarp::os::RpcServer::interrupt() {
    port.interrupt();
}

void yarp::os::RpcServer::resume() {
    port.resume();
}

yarp::os::Contact yarp::os::RpcServer::where() const {
    return port.where();
}

yarp::os::ConstString yarp::os::RpcServer::getName() const {
    return where().getName();
}

bool yarp::os::RpcServer::read(PortReader& reader, bool willReply) {
    if (!willReply) {
        // this is an error for RpcServer
        return false;
    }
    return port.read(reader,true);
}

bool yarp::os::RpcServer::reply(PortWriter& writer) {
    return port.reply(writer);
}

void yarp::os::RpcServer::setReader(PortReader& reader) {
    port.setReader(reader);
}

void yarp::os::RpcServer::setReaderCreator(PortReaderCreator& creator) {
    port.setReaderCreator(creator);
}

bool yarp::os::RpcServer::setEnvelope(PortWriter& envelope) {
    return port.setEnvelope(envelope);
}

bool yarp::os::RpcServer::getEnvelope(PortReader& envelope) {
    return port.getEnvelope(envelope);
}

int yarp::os::RpcServer::getInputCount() {
    return port.getInputCount();
}

int yarp::os::RpcServer::getOutputCount() {
    return port.getOutputCount();
}

void yarp::os::RpcServer::getReport(PortReport& reporter) {
    port.getReport(reporter);
}

void yarp::os::RpcServer::setReporter(PortReport& reporter) {
    port.setReporter(reporter);
}

bool yarp::os::RpcServer::isWriting() {
    return port.isWriting();
}
