/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_MODULE_H
#define ROBOTINTERFACE_MODULE_H

#include <yarp/os/RFModule.h>
#include <robotInterfaceRpc.h>

namespace RobotInterface
{

class Module : public yarp::os::RFModule,
               public robotInterfaceRpc
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
    virtual bool attach(yarp::os::RpcServer &source);

    // robotInterfaceRpc
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

#endif // ROBOTINTERFACE_MODULE_H
