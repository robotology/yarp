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

namespace yarp
{
    namespace dev
    {
        class JoypadControlServer;
    }
}

//-----Openable and JoyPort are for confortable loop managing of ports
struct Openable
{
    bool valid;

    virtual ~Openable(){}
    virtual bool open()      = 0;
    virtual void interrupt() = 0;
    virtual void close()     = 0;
    virtual void write()     = 0;
};

template <typename T>
struct JoyPort : public Openable
{

    yarp::os::BufferedPort<T> port;
    yarp::os::ConstString     name;

    T& prepare(){return port.prepare();}
    bool open()      YARP_OVERRIDE {return port.open(name);}
    void interrupt() YARP_OVERRIDE {port.interrupt();}
    void close()     YARP_OVERRIDE {port.close();}
    void write()     YARP_OVERRIDE {port.write();}

};
//----------


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


    unsigned int                    m_rate;
    JoypadCtrlParser                m_parser;
    yarp::dev::IJoypadController*   m_device;
    yarp::os::Port                  m_rpcPort;
    yarp::dev::PolyDriver*          m_subDeviceOwned;
    bool                            m_isSubdeviceOwned;
    bool                            m_separatePorts;
    yarp::os::ConstString           m_rpcPortName;
    JoyPort<Vector>                 m_portAxis;
    JoyPort<Vector>                 m_portStick;
    JoyPort<Vector>                 m_portTouch;
    JoyPort<Vector>                 m_portButtons;
    JoyPort<VecOfChar>              m_portHats;
    JoyPort<Vector>                 m_portTrackball;
    yarp::os::BufferedPort<JoyData> m_godPort;
    coordsMode                      m_coordsMode;


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


