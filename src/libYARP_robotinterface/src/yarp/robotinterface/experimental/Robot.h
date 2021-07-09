/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_ROBOT_H
#define YARP_ROBOTINTERFACE_ROBOT_H

#include <yarp/robotinterface/api.h>
#include <yarp/robotinterface/experimental/Device.h>
#include <yarp/robotinterface/experimental/Types.h>

namespace yarp {
namespace robotinterface {
namespace experimental {

class YARP_robotinterface_API Robot
{
public:
    explicit Robot();
    explicit Robot(const std::string& name, const DeviceList& devices = DeviceList());
    Robot(const Robot& other);
    Robot& operator=(const Robot& other);

    virtual ~Robot();

    std::string& name();
    unsigned int& build();
    std::string& portprefix();

    void setVerbose(bool verbose);
    void setAllowDeprecatedDevices(bool allowDeprecatedDevices);

    ParamList& params();
    DeviceList& devices();
    Device& device(const std::string& name);

    const std::string& name() const;
    const unsigned int& build() const;
    const std::string& portprefix() const;
    const ParamList& params() const;

    const DeviceList& devices() const;
    bool hasDevice(const std::string& name) const;
    const Device& device(const std::string& name) const;

    bool hasParam(const std::string& name) const;
    std::string findParam(const std::string& name) const;

    void interrupt();
    bool setExternalDevices(const yarp::dev::PolyDriverList& list);
    bool enterPhase(yarp::robotinterface::experimental::ActionPhase phase);
    yarp::robotinterface::experimental::ActionPhase currentPhase() const;
    int currentLevel() const;

private:
    class Private;
    Private* const mPriv;
}; // class Robot

} // namespace experimental
} // namespace robotinterface
} // namespace yarp

YARP_robotinterface_API yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::experimental::Robot& t);


#endif // YARP_ROBOTINTERFACE_ROBOT_H
