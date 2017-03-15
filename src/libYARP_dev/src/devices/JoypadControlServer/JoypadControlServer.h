/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    yarp::dev::IJoypadController* device;
public:
    JoypadCtrlParser();
    virtual ~JoypadCtrlParser(){}

    bool         configure(yarp::dev::IJoypadController* interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};

class yarp::dev::JoypadControlServer: public yarp::dev::DeviceDriver,
                                      public yarp::dev::IWrapper,
                                      public yarp::dev::IMultipleWrapper,
                                      public yarp::os::RateThread
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    typedef yarp::os::BufferedPort<yarp::os::Bottle> BottlePort;
    typedef yarp::dev::IJoypadController::JoypadCtrl_coordinateMode coordsMode;
    struct JoyPort
    {
        yarp::os::ConstString name;
        BottlePort            port;
        bool                  valid;

        bool              open()     {return port.open(name);}
        void              interrupt(){port.interrupt();}
        void              close()    {port.close();}
        yarp::os::Bottle& prepare()  {return port.prepare();}
        void              write()    {port.write();}

    };

    unsigned int                  m_rate;
    JoypadCtrlParser              m_parser;
    yarp::dev::IJoypadController* m_device;
    yarp::os::Port                m_rpcPort;
    yarp::dev::PolyDriver*        m_subDeviceOwned;
    bool                          m_isSubdeviceOwned;
    yarp::os::ConstString         m_rpcPortName;
    JoyPort                       m_portAxis;
    JoyPort                       m_portStick;
    JoyPort                       m_portTouch;
    JoyPort                       m_portButtons;
    JoyPort                       m_portHats;
    JoyPort                       m_portTrackball;
    coordsMode                    m_coordsMode;


    bool openAndAttachSubDevice(yarp::os::Searchable& prop);
    bool openPorts();

#endif //DOXYGEN_SHOULD_SKIP_THIS
public:
    JoypadControlServer();
    ~JoypadControlServer();

    bool        open(yarp::os::Searchable& params);
    bool        fromConfig(yarp::os::Searchable& params);
    bool        close();

    bool        attachAll(const yarp::dev::PolyDriverList& p);
    bool        detachAll();
    bool        attach(yarp::dev::PolyDriver* poly);
    bool        attach(yarp::dev::IJoypadController* s);
    bool        detach();
    bool        threadInit();
    void        threadRelease();
    void        run();
};

#endif


