/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};

//TODO: finish the single port mode.. the struct below is for this purpose
struct JoyData : public yarp::os::Portable
{
    yarp::sig::Vector Buttons;
    yarp::sig::Vector Sticks;
    yarp::sig::Vector Axes;
    yarp::sig::Vector Balls;
    yarp::sig::Vector Touch;
    yarp::sig::VectorOf<unsigned char> Hats;

    bool read(yarp::os::ConnectionReader& connection)
    {
        Buttons.resize(connection.expectInt());
        Sticks.resize(connection.expectInt());
        Axes.resize(connection.expectInt());
        Balls.resize(connection.expectInt());
        Touch.resize(connection.expectInt());
        Hats.resize(connection.expectInt());
        connection.expectBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
        connection.expectBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
        connection.expectBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
        connection.expectBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
        connection.expectBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
        connection.expectBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
        return !connection.isError();
    }

    bool write(yarp::os::ConnectionWriter& connection)
    {
        connection.appendInt(Buttons.length());
        connection.appendInt(Sticks.length());
        connection.appendInt(Axes.length()  );
        connection.appendInt(Balls.length() );
        connection.appendInt(Touch.length() );
        connection.appendInt(Hats.size()  );
        connection.appendBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
        connection.appendBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
        connection.appendBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
        connection.appendBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
        connection.appendBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
        connection.appendBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
        connection.convertTextMode();
        return !connection.isError();
    }
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

    bool open(yarp::os::Searchable& params);
    bool fromConfig(yarp::os::Searchable& params);
    bool close();
    bool attachAll(const yarp::dev::PolyDriverList& p);
    bool detachAll();
    bool attach(yarp::dev::PolyDriver* poly);
    bool attach(yarp::dev::IJoypadController* s);
    bool detach();
    bool threadInit();
    void threadRelease();
    void run();

    #undef JoyPort
};

#endif


