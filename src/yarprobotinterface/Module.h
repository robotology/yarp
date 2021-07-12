/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_YARPROBOTINTERFACE_MODULE_H
#define YARP_YARPROBOTINTERFACE_MODULE_H

#include <yarp/os/RFModule.h>
#include <yarprobotinterfaceRpc.h>

namespace yarprobotinterface
{

class Module : public yarp::os::RFModule,
               public yarprobotinterfaceRpc
{
public:
    explicit Module();
    virtual ~Module();

    // yarp::os::RFModule
    double getPeriod() override;
    bool updateModule() override;
    bool close() override;
    bool interruptModule() override;
    bool configure(yarp::os::ResourceFinder &rf) override;

    using yarp::os::RFModule::attach;
    bool attach(yarp::os::RpcServer &source) override;

    // yarprobotinterfaceRpc
    std::string get_phase() override;
    int32_t get_level() override;
    bool is_ready() override;
    std::string get_robot() override;

    std::string quit() override;
    inline std::string bye() override { return quit(); }
    inline std::string exit() override { return quit(); }

private:
    class Private;
    Private * const mPriv;
}; // class Module

} // namespace RobotInterface

#endif // YARP_YARPROBOTINTERFACE_MODULE_H
