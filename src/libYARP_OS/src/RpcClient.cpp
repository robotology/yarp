// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcClient.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;


RpcClient::RpcClient() {
}

RpcClient::~RpcClient() {
    port.close();
}

bool RpcClient::open(const ConstString& name) {
    port.setInputMode(false);
    port.setOutputMode(true);
    port.setRpcMode(true);
    return port.open(name);
}

bool RpcClient::open(const Contact& contact, bool registerName) {
    return port.open(contact,registerName);
}

bool RpcClient::addOutput(const ConstString& name) {
    return port.addOutput(name);
}

bool RpcClient::addOutput(const ConstString& name, 
                                    const ConstString& carrier) {
    return port.addOutput(name,carrier);
}

bool RpcClient::addOutput(const Contact& contact){
    return port.addOutput(contact);
}

void RpcClient::close() {
    port.close();
}

void RpcClient::interrupt() {
    port.interrupt();
}

void RpcClient::resume() {
    port.resume();
}

Contact RpcClient::where() const {
    return port.where();
}

ConstString RpcClient::getName() const {
    return where().getName();
}

bool RpcClient::write(PortWriter& writer, PortWriter *callback) const {
    YARP_SPRINTF1(Logger::get(),error,"message written to RpcClient %s without a place for a reply, please use a regular Port for that",port.getName().c_str());
    return false;
}

bool RpcClient::write(PortWriter& writer, PortReader& reader,
                                PortWriter *callback) const {
    return port.write(writer,reader,callback);
}


bool RpcClient::read(PortReader& reader, bool willReply) {
    YARP_SPRINTF1(Logger::get(),error,"cannot read from RpcClient %s, please use a regular Port for that",port.getName().c_str());
    return false;
}

bool RpcClient::reply(PortWriter& writer) {
    return false;
}

bool RpcClient::replyAndDrop(PortWriter& writer) {
    return false;
}

bool RpcClient::setEnvelope(PortWriter& envelope) {
    return port.setEnvelope(envelope);
}

bool RpcClient::getEnvelope(PortReader& envelope) {
    return port.getEnvelope(envelope);
}

int RpcClient::getInputCount() {
    return port.getInputCount();
}

int RpcClient::getOutputCount() {
    return port.getOutputCount();
}

void RpcClient::getReport(PortReport& reporter) {
    port.getReport(reporter);
}

void RpcClient::setReporter(PortReport& reporter) {
    port.setReporter(reporter);
}

bool RpcClient::isWriting() {
    return port.isWriting();
}

void RpcClient::setReader(PortReader& reader) {
    port.setReader(reader);
}


Type RpcClient::getType() {
    return port.getType();
}

void RpcClient::promiseType(const Type& typ) {
    port.promiseType(typ);
}


Property *RpcClient::acquireProperties(bool readOnly) {
    return port.acquireProperties(readOnly);
}

void RpcClient::releaseProperties(Property *prop) {
    port.releaseProperties(prop);
}


void RpcClient::setInputMode(bool expectInput) {
    YARP_ASSERT(!expectInput);
}


void RpcClient::setOutputMode(bool expectOutput) {
    YARP_ASSERT(expectOutput);
}


void RpcClient::setRpcMode(bool expectRpc) {
    YARP_ASSERT(expectRpc);
}

