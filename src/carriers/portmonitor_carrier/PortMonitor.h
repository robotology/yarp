// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef PORTMONITOR_INC
#define PORTMONITOR_INC

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Things.h>

#include "MonitorBinding.h"
#include "MonitorEvent.h"

namespace yarp {
    namespace os {
        class PortMonitor;
        class PortMonitorGroup;
    }
}

/**
 *
 * Manager for arbitration-aware inputs to a given port.
 *
 */

class yarp::os::PortMonitorGroup : public PeerRecord<PortMonitor> {
public:
    virtual ~PortMonitorGroup() {}
    virtual bool acceptIncomingData(PortMonitor *source);
};



/**
 *
 * Allow to monitor and modify port data from Lua script  Under development.
 * Affected by carrier modifiers.  
 *
 * Examples: tcp+recv.portmonitor+type.lua+file.my_lua_script_file
 *
 */

/**
 * TODO: 
 *      - how to pass the filename with prefix via connection paramter? 
 *        e.g. ...+file.'my_lua_script_file.lua' 
 *      - using resource finder to find the script file  
 *
 */
class yarp::os::PortMonitor : public yarp::os::ModifyingCarrier
{

public:
    PortMonitor(){
        bReady = false;
        binder = NULL;        
        group = NULL;
        localReader = NULL;
    }

    virtual ~PortMonitor() {
        if (portName!="") {
            getPeers().remove(portName,this);
        }
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
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader);

    virtual yarp::os::PortWriter& modifyOutgoingData(yarp::os::PortWriter& writer);

    virtual bool acceptOutgoingData(yarp::os::PortWriter& wrtier);

    virtual yarp::os::PortReader& modifyReply(yarp::os::PortReader& reader);

    virtual void setCarrierParams(const yarp::os::Property& params);

    virtual void getCarrierParams(yarp::os::Property& params);


    void lock() { mutex.wait(); }
    void unlock() { mutex.post(); }

    MonitorBinding* getBinder(void) {
        if(!bReady)
            return NULL;
        return binder;
    }

public:
    ConstString portName;
    ConstString sourceName;

private:    
    static ElectionOf<PortMonitorGroup> *peers;
    static ElectionOf<PortMonitorGroup>& getPeers();


private:
    bool happy;
    bool bReady;
    yarp::os::DummyConnector con;
    yarp::os::ConnectionReader* localReader;
    yarp::os::Things thing;
    MonitorBinding* binder;
    PortMonitorGroup *group;    
    yarp::os::Semaphore mutex; 
};

#endif //PORTMONITOR_INC


