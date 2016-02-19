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
    virtual double getPeriod();
    virtual bool updateModule();
    virtual bool close();
    virtual bool interruptModule();
    virtual bool configure(yarp::os::ResourceFinder &rf);

    using yarp::os::RFModule::attach;
    virtual bool attach(yarp::os::RpcServer &source);

    // yarprobotinterfaceRpc
    virtual std::string get_phase();
    virtual int32_t get_level();
    virtual bool is_ready();
    virtual std::string get_robot();

    virtual std::string quit();
    inline virtual std::string bye() { return quit(); }
    inline virtual std::string exit() { return quit(); }

private:
    class Private;
    Private * const mPriv;
}; // class Module

} // namespace RobotInterface

#endif // YARP_YARPROBOTINTERFACE_MODULE_H
