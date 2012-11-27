/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_MODULE_H
#define ROBOTINTERFACE_MODULE_H

#include <yarp/os/Module.h>

namespace RobotInterface
{

class Module : public yarp::os::RFModule
{
public:
    explicit Module();
    virtual ~Module();

    virtual double getPeriod();

    virtual bool updateModule();
    virtual bool close();
    virtual bool interruptModule();
    virtual bool configure(yarp::os::ResourceFinder &rf);

private:
    class Private;
    Private * const mPriv;
}; // class Module

} // namespace RobotInterface

#endif // ROBOTINTERFACE_MODULE_H
