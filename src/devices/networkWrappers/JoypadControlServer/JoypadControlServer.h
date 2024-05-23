/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H
#define YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <map>
#include <JoypadControlNetUtils.h>
#include "JoypadControlServer_ParamsParser.h"

class JoypadCtrlParser :
        public yarp::dev::DeviceResponder
{
private:
    typedef bool (yarp::dev::IJoypadController::*getcountmethod)(unsigned int&);

    std::map<int, getcountmethod> m_countGetters;
    yarp::dev::IJoypadController* m_device;
public:
    JoypadCtrlParser();
    ~JoypadCtrlParser() override = default;

    bool         configure(yarp::dev::IJoypadController* interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

/**
 * @ingroup dev_impl_wrapper
 *
 * `JoypadControlServer`: joypad input network wrapper on server side
 *
 * \section JoypadControlServer Description of input parameters
 *
 * Parameters required by this device are shown in class: JoypadControlServer_ParamsParser
 */
class JoypadControlServer :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public yarp::dev::IService,
        public JoypadControlServer_ParamsParser
{
    typedef yarp::dev::IJoypadController::JoypadCtrl_coordinateMode coordsMode;
    typedef yarp::sig::Vector                  Vector;
    typedef yarp::sig::VectorOf<unsigned char> VecOfChar;

    template<typename T>
    using JoyPort = JoypadControl::JoyPort<T>;

    JoypadCtrlParser                m_parser;
    yarp::dev::IJoypadController*   m_IJoypad = nullptr;
    yarp::os::Port                  m_rpcPort;
    std::string           m_rpcPortName;
    JoyPort<Vector>                 m_portAxis;
    JoyPort<Vector>                 m_portStick;
    JoyPort<Vector>                 m_portTouch;
    JoyPort<Vector>                 m_portButtons;
    JoyPort<VecOfChar>              m_portHats;
    JoyPort<Vector>                 m_portTrackball;
    yarp::os::BufferedPort<JoyData> m_godPort; //TODO: single port purpose
    coordsMode                      m_coordsMode = yarp::dev::IJoypadController::JypCtrlcoord_CARTESIAN;
    double m_lastRunTime = 0.0;


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
    bool close() override;
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;
    bool startService() override;
    bool updateService() override;
    bool stopService() override;
};

#endif
