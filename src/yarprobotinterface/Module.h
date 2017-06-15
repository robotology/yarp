/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_YARPROBOTINTERFACE_MODULE_H
#define YARP_YARPROBOTINTERFACE_MODULE_H

#include <yarp/os/RFModule.h>
#include <yarprobotinterfaceRpc.h>

namespace RobotInterface
{

class Module : public yarp::os::RFModule,
               public yarprobotinterfaceRpc
{
public:
    explicit Module();
    virtual ~Module();

    // yarp::os::RFModule
    virtual double getPeriod() override;
    virtual bool updateModule() override;
    virtual bool close() override;
    virtual bool interruptModule() override;
    virtual bool configure(yarp::os::ResourceFinder &rf) override;

    using yarp::os::RFModule::attach;
    virtual bool attach(yarp::os::RpcServer &source) override;

    // yarprobotinterfaceRpc
    virtual std::string get_phase() override;
    virtual int32_t get_level() override;
    virtual bool is_ready() override;
    virtual std::string get_robot() override;

    virtual std::string quit() override;
    inline virtual std::string bye() override { return quit(); }
    inline virtual std::string exit() override { return quit(); }

private:
    class Private;
    Private * const mPriv;
}; // class Module

} // namespace RobotInterface

#endif // YARP_YARPROBOTINTERFACE_MODULE_H
