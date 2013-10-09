// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef PORTMONITOR_INC
#define PORTMONITOR_INC

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/DummyConnector.h>

#include "MonitorBinding.h"

namespace yarp {
    namespace os {
        class PortMonitor;
    }
}


/**
 *
 * Allow to monitor and modify port data from Lua script  Under development.
 * Affected by carrier modifiers.  
 *
 * Examples: tcp+recv.portmonitor+script.lua+file.my_lua_script_file
 *
 */

/**
 * TODO: 
 *      - how to pass the filename with prefix via connection paramter? 
 *        e.g. ...+file.'my_lua_script_file.lua' 
 *      - using resource finder to find the script file  
 *
 */
class yarp::os::PortMonitor : public yarp::os::ModifyingCarrier,
                               public yarp::os::ConnectionReader,
                               public yarp::os::InputStream {

public:
    PortMonitor(){
        local = yarp::os::ConnectionReader::createConnectionReader(*this);
        happy = (local!=0);
        bReady = false;
        binder = NULL;
    }

    virtual ~PortMonitor() {
        if (local) delete local;
        if (binder) delete binder;
    }

    virtual Carrier *create() {
        return new PortMonitor();
    }

    virtual ConstString getName() {
        return "portmonitor";
    }

    virtual ConstString toString() {
        return "portmonitor_carrier";
    }

    virtual bool configure(yarp::os::ConnectionState& proto);
    
    //virtual bool modifiesIncomingData();
    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader);

    virtual void setCarrierParams(const yarp::os::Property& params);

    virtual void getCarrierParams(yarp::os::Property& params);


    void setParent(yarp::os::ConnectionReader& reader) {
        parent = &reader;
    }

    virtual bool expectBlock(const char *data, size_t len) {
        return local->expectBlock(data,len);
    }

    virtual ConstString expectText(int terminatingChar = '\n') {
        return local->expectText(terminatingChar);
    }

    virtual int expectInt() {
        return local->expectInt();
    }

    virtual bool pushInt(int x) {
        return local->pushInt(x);
    }

    virtual double expectDouble() {
        return local->expectDouble();
    }

    virtual bool isTextMode() {
        return false;
    }

    virtual bool convertTextMode() {
        return true;
    }

    virtual size_t getSize() {
        return 0;
    }

    virtual ConnectionWriter *getWriter() {
        return parent->getWriter();
    }

    virtual Bytes readEnvelope() { 
        return parent->readEnvelope();
    }

    virtual Portable *getReference() {
        return parent->getReference();
    }

    virtual Contact getRemoteContact() {
        return parent->getRemoteContact();
    }

    virtual Contact getLocalContact() {
        return parent->getLocalContact();
    }

    virtual bool isValid() {
        return true;
    }

    virtual bool isActive() {
        return parent->isActive();
    }

    virtual bool isError() {
        return parent->isError()||!happy;
    }

    virtual void requestDrop() {
        parent->requestDrop();
    }

    virtual yarp::os::Searchable& getConnectionModifiers() {
        return parent->getConnectionModifiers();
    }

    virtual bool setSize(size_t len) {
        return parent->setSize(len);
    }

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);


    ////////////////////////////////////////////////////////////////////////
    // InputStream methods
    
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b);

    virtual void close() {
    }

    virtual bool isOk() {
        return happy;
    }    

private:
    bool happy;
    bool bReady;
    ConstString portName;
    ConstString sourceName;
    yarp::os::ConnectionReader *local;
    yarp::os::ConnectionReader *parent;
    yarp::os::DummyConnector con;

    MonitorBinding* binder;
};

#endif //PORTMONITOR_INC


