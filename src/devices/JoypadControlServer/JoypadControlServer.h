/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H
#define YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <map>
#include <JoypadControlNetUtils.h>


class JoypadCtrlParser :
        public yarp::dev::DeviceResponder
{
private:
    typedef bool (yarp::dev::IJoypadController::*getcountmethod)(unsigned int&);

    std::map<int, getcountmethod> countGetters;
    yarp::dev::IJoypadController* device;
public:
    JoypadCtrlParser();
    ~JoypadCtrlParser() override = default;

    bool         configure(yarp::dev::IJoypadController* interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

/**
 * @ingroup dev_impl_wrapper
 *
 * `JoypadControlServer`: Documentation to be added
 */
class JoypadControlServer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PeriodicThread
{
    typedef yarp::dev::IJoypadController::JoypadCtrl_coordinateMode coordsMode;
    typedef yarp::sig::Vector                  Vector;
    typedef yarp::sig::VectorOf<unsigned char> VecOfChar;

    template<typename T>
    using JoyPort = JoypadControl::JoyPort<T>;

    double                          m_period;
    JoypadCtrlParser                m_parser;
    yarp::dev::IJoypadController*   m_device;
    yarp::os::Port                  m_rpcPort;
    yarp::dev::PolyDriver*          m_subDeviceOwned;
    bool                            m_isSubdeviceOwned;
    bool                            m_separatePorts;
    bool                            m_profile;
    std::string           m_rpcPortName;
    std::string           m_name;
    JoyPort<Vector>                 m_portAxis;
    JoyPort<Vector>                 m_portStick;
    JoyPort<Vector>                 m_portTouch;
    JoyPort<Vector>                 m_portButtons;
    JoyPort<VecOfChar>              m_portHats;
    JoyPort<Vector>                 m_portTrackball;
    yarp::os::BufferedPort<JoyData> m_godPort; //TODO: single port purpose
    coordsMode                      m_coordsMode;


    bool openAndAttachSubDevice(yarp::os::Searchable& prop);
    bool openPorts();
    void profile();

public:
    JoypadControlServer();
    JoypadControlServer(const JoypadControlServer&) = delete;
    JoypadControlServer(JoypadControlServer&&) = delete;
    JoypadControlServer& operator=(const JoypadControlServer&) = delete;
    JoypadControlServer& operator=(JoypadControlServer&&) = delete;
    ~JoypadControlServer() override;

    bool open(yarp::os::Searchable& params) override;
    bool fromConfig(yarp::os::Searchable& params);
    bool close() override;
    bool attachAll(const yarp::dev::PolyDriverList& p) override;
    bool detachAll() override;
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool attach(yarp::dev::IJoypadController* s);
    bool detach() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;
};

#endif
