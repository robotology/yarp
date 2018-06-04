/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef PORTMONITOR_INC
#define PORTMONITOR_INC

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/Things.h>

#include "MonitorBinding.h"
#include "MonitorEvent.h"

#include <mutex>

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
 *      - how to pass the filename with prefix via connection parameter?
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

    virtual Carrier *create() override {
        return new PortMonitor();
    }

    virtual std::string getName() const override {
        return "portmonitor";
    }

    virtual std::string toString() override {
        return "portmonitor_carrier";
    }

    virtual bool configure(yarp::os::ConnectionState& proto) override;
    virtual bool configureFromProperty(yarp::os::Property& options) override;

    //virtual bool modifiesIncomingData() override;
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) override;

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;

    virtual yarp::os::PortWriter& modifyOutgoingData(yarp::os::PortWriter& writer) override;

    virtual bool acceptOutgoingData(yarp::os::PortWriter& wrtier) override;

    virtual yarp::os::PortReader& modifyReply(yarp::os::PortReader& reader) override;

    virtual void setCarrierParams(const yarp::os::Property& params) override;

    virtual void getCarrierParams(yarp::os::Property& params) const override;


    void lock() const { mutex.lock(); }
    void unlock() const { mutex.unlock(); }

    MonitorBinding* getBinder(void) {
        if(!bReady)
            return NULL;
        return binder;
    }

public:
    std::string portName;
    std::string sourceName;

private:
    static ElectionOf<PortMonitorGroup> *peers;
    static ElectionOf<PortMonitorGroup>& getPeers();


private:
    bool bReady;
    yarp::os::DummyConnector con;
    yarp::os::ConnectionReader* localReader;
    yarp::os::Things thing;
    MonitorBinding* binder;
    PortMonitorGroup *group;
    mutable std::mutex mutex;
};

#endif //PORTMONITOR_INC


