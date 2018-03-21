/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H
#define YARP_DEV_JOYPADCONTROLSERVER_JOYPADCONTROLSERVER_H

#include <yarp/os/RateThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <map>
#include "JoypadControlNetUtils.h"

namespace yarp
{
    namespace dev
    {
        class JoypadControlServer;
    }
}




class JoypadCtrlParser: public yarp::dev::DeviceResponder
{
private:
    typedef bool (yarp::dev::IJoypadController::*getcountmethod)(unsigned int&);

    std::map<int, getcountmethod> countGetters;
    yarp::dev::IJoypadController* device;
public:
    JoypadCtrlParser();
    virtual ~JoypadCtrlParser(){}

    bool         configure(yarp::dev::IJoypadController* interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

class yarp::dev::JoypadControlServer: public yarp::dev::DeviceDriver,
                                      public yarp::dev::IWrapper,
                                      public yarp::dev::IMultipleWrapper,
                                      public yarp::os::RateThread
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    typedef yarp::dev::IJoypadController::JoypadCtrl_coordinateMode coordsMode;
    typedef yarp::sig::Vector                  Vector;
    typedef yarp::sig::VectorOf<unsigned char> VecOfChar;
    #define JoyPort yarp::dev::JoypadControl::JoyPort


    unsigned int                    m_rate;
    JoypadCtrlParser                m_parser;
    yarp::dev::IJoypadController*   m_device;
    yarp::os::Port                  m_rpcPort;
    yarp::dev::PolyDriver*          m_subDeviceOwned;
    bool                            m_isSubdeviceOwned;
    bool                            m_separatePorts;
    bool                            m_profile;
    yarp::os::ConstString           m_rpcPortName;
    yarp::os::ConstString           m_name;
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

#endif //DOXYGEN_SHOULD_SKIP_THIS

public:
    JoypadControlServer();
    ~JoypadControlServer();

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

    #undef JoyPort
};

#endif


