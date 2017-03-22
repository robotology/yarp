/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/RateThread.h>
#include <vector>
#include <JoypadControlNetUtils.h>

#define DEFAULT_THREAD_PERIOD 10
namespace yarp
{
    namespace dev
    {
        class JoypadControlClient;
    }
}

class yarp::dev::JoypadControlClient : public yarp::dev::IJoypadController,
                                       public yarp::dev::DeviceDriver
{
private:
    //---------------utils
    #define JOYPORT yarp::dev::JoypadControl::JoyPort
    typedef yarp::sig::Vector                  Vector;
    typedef yarp::sig::VectorOf<unsigned char> CharVector;

    //---------------properties
    yarp::os::Port        m_rpcPort;
    JOYPORT<Vector>       m_buttonsPort;
    JOYPORT<Vector>       m_axisPort;
    JOYPORT<Vector>       m_trackballPort;
    JOYPORT<Vector>       m_touchPort;
    JOYPORT<CharVector>   m_hatsPort;
    JOYPORT<Vector>       m_stickPort;
    bool                  m_rpc_only;
    yarp::os::ConstString m_local;
    yarp::os::ConstString m_remote;
    yarp::os::Mutex       m_mutex;

    //--------------method
    bool getCount(const int& vocab_toget, unsigned int& value);
    bool getJoypadInfo();

public:
    JoypadControlClient();

    //rateThread

    //DeviceDriver
    virtual bool open(yarp::os::Searchable& config) YARP_OVERRIDE;
    virtual bool close() YARP_OVERRIDE;

    //IJoypadController;
    virtual bool getAxisCount(unsigned int& axis_count) YARP_OVERRIDE;
    virtual bool getButtonCount(unsigned int& button_count) YARP_OVERRIDE;
    virtual bool getTrackballCount(unsigned int& Trackball_count) YARP_OVERRIDE;
    virtual bool getHatCount(unsigned int& Hat_count) YARP_OVERRIDE;
    virtual bool getTouchSurfaceCount(unsigned int& touch_count) YARP_OVERRIDE;
    virtual bool getStickCount(unsigned int& stick_count) YARP_OVERRIDE;
    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF) YARP_OVERRIDE;
    virtual bool getButton(unsigned int button_id, float& value) YARP_OVERRIDE;
    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) YARP_OVERRIDE;
    virtual bool getHat(unsigned int hat_id, unsigned char& value) YARP_OVERRIDE;
    virtual bool getAxis(unsigned int axis_id, double& value) YARP_OVERRIDE;
    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) YARP_OVERRIDE;
    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) YARP_OVERRIDE;

};
