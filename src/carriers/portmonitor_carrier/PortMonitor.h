/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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


class PortMonitor;

/**
 * Manager for arbitration-aware inputs to a given port.
 */
class PortMonitorGroup :
        public yarp::os::PeerRecord<PortMonitor>
{
public:
    virtual ~PortMonitorGroup() {}
    virtual bool acceptIncomingData(PortMonitor *source);
};



/**
 * Allow to monitor and modify port data from Lua script  Under development.
 * Affected by carrier modifiers.
 *
 * Examples: tcp+recv.portmonitor+type.lua+file.my_lua_script_file
 */

/**
 * TODO:
 *      - how to pass the filename with prefix via connection parameter?
 *        e.g. ...+file.'my_lua_script_file.lua'
 *      - using resource finder to find the script file
 */
class PortMonitor :
        public yarp::os::ModifyingCarrier
{

public:
    PortMonitor() = default;
    PortMonitor(const PortMonitor&) = delete;
    PortMonitor(PortMonitor&&) = delete;
    PortMonitor& operator=(const PortMonitor&) = delete;
    PortMonitor& operator=(PortMonitor&&) = delete;

    ~PortMonitor() override
    {
        if (!portName.empty()) {
            getPeers().remove(portName,this);
        }
        delete binder;
    }

    Carrier *create() const override
    {
        return new PortMonitor();
    }

    std::string getName() const override
    {
        return "portmonitor";
    }

    std::string toString() const override
    {
        return "portmonitor_carrier";
    }

    bool configure(yarp::os::ConnectionState& proto) override;
    bool configureFromProperty(yarp::os::Property& options) override;

    //bool modifiesIncomingData() override;
    bool acceptIncomingData(yarp::os::ConnectionReader& reader) override;

    yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;

    const yarp::os::PortWriter& modifyOutgoingData(const yarp::os::PortWriter& writer) override;

    bool acceptOutgoingData(const yarp::os::PortWriter& writer) override;

    yarp::os::PortReader& modifyReply(yarp::os::PortReader& reader) override;

    void setCarrierParams(const yarp::os::Property& params) override;

    void getCarrierParams(yarp::os::Property& params) const override;


    void lock() const { mutex.lock(); }
    void unlock() const { mutex.unlock(); }

    MonitorBinding* getBinder()
    {
        if(!bReady) {
            return nullptr;
        }
        return binder;
    }

public:
    std::string portName;
    std::string sourceName;

private:
    static yarp::os::ElectionOf<PortMonitorGroup> *peers;
    static yarp::os::ElectionOf<PortMonitorGroup>& getPeers();


private:
    bool bReady {false};
    yarp::os::DummyConnector con;
    yarp::os::ConnectionReader* localReader {nullptr};
    yarp::os::Things thing;
    MonitorBinding* binder {nullptr};
    PortMonitorGroup *group {nullptr};
    mutable std::mutex mutex;
};

#endif //PORTMONITOR_INC
