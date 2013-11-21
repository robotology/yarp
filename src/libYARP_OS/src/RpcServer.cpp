// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcServer.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

RpcServer::RpcServer() {
}

RpcServer::~RpcServer() {
    port.close();
}

bool RpcServer::open(const ConstString& name) {
    port.setInputMode(true);
    port.setOutputMode(false);
    port.setRpcMode(true);
    return port.open(name);
}

bool RpcServer::open(const Contact& contact, bool registerName) {
    return port.open(contact,registerName);
}

bool RpcServer::addOutput(const ConstString& name) {
    return false;
}

bool RpcServer::addOutput(const ConstString& name, 
                                    const ConstString& carrier) {
    return false;
}

bool RpcServer::addOutput(const Contact& contact){
    return false;
}

void RpcServer::close() {
    port.close();
}

void RpcServer::interrupt() {
    port.interrupt();
}

void RpcServer::resume() {
    port.resume();
}

Contact RpcServer::where() const {
    return port.where();
}

ConstString RpcServer::getName() const {
    return where().getName();
}

bool RpcServer::write(PortWriter& writer, PortWriter *callback) const {
    YARP_SPRINTF1(Logger::get(),error,"RpcServer %s cannot write, please use a regular Port or RpcClient for that",port.getName().c_str());
    return false;
}

bool RpcServer::write(PortWriter& writer, PortReader& reader,
                                PortWriter *callback) const {
    YARP_SPRINTF1(Logger::get(),error,"RpcServer %s cannot write, please use a regular Port or RpcClient for that",port.getName().c_str());
    return false;
}

bool RpcServer::read(PortReader& reader, bool willReply) {
    if (!willReply) {
        YARP_SPRINTF1(Logger::get(),error,"RpcServer %s must reply, please use a regular Port if you do not want to",port.getName().c_str());
        // this is an error for RpcServer
        return false;
    }
    return port.read(reader,true);
}

bool RpcServer::reply(PortWriter& writer) {
    return port.reply(writer);
}

bool RpcServer::replyAndDrop(PortWriter& writer) {
    return port.replyAndDrop(writer);
}

void RpcServer::setReader(PortReader& reader) {
    port.setReader(reader);
}

void RpcServer::setReaderCreator(PortReaderCreator& creator) {
    port.setReaderCreator(creator);
}

bool RpcServer::setEnvelope(PortWriter& envelope) {
    return port.setEnvelope(envelope);
}

bool RpcServer::getEnvelope(PortReader& envelope) {
    return port.getEnvelope(envelope);
}

int RpcServer::getInputCount() {
    return port.getInputCount();
}

int RpcServer::getOutputCount() {
    return port.getOutputCount();
}

void RpcServer::getReport(PortReport& reporter) {
    port.getReport(reporter);
}

void RpcServer::setReporter(PortReport& reporter) {
    port.setReporter(reporter);
}

bool RpcServer::isWriting() {
    return port.isWriting();
}

Type RpcServer::getType() {
    return port.getType();
}

void RpcServer::promiseType(const Type& typ) {
    port.promiseType(typ);
}

Property *RpcServer::acquireProperties(bool readOnly) {
    return port.acquireProperties(readOnly);
}

void RpcServer::releaseProperties(Property *prop) {
    port.releaseProperties(prop);
}

void RpcServer::setInputMode(bool expectInput) {
    YARP_ASSERT(expectInput);
}


void RpcServer::setOutputMode(bool expectOutput) {
    YARP_ASSERT(!expectOutput);
}


void RpcServer::setRpcMode(bool expectRpc) {
    YARP_ASSERT(expectRpc);
}

