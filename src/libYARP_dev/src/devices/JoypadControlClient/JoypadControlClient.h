/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/RateThread.h>
#include <vector>

#define DEFAULT_THREAD_PERIOD 10
namespace yarp
{
    namespace dev
    {
        class JoypadControlClient;
    }
}

class yarp::dev::JoypadControlClient : public yarp::dev::IJoypadController,
                                       public yarp::dev::DeviceDriver,
                                       public yarp::os::RateThread
{
    typedef yarp::sig::Vector Vector;
    typedef yarp::os::BufferedPort<yarp::sig::VectorOf<unsigned char> > hatportType;

    yarp::os::Port                 m_rpcPort;
    yarp::os::BufferedPort<Vector> m_buttonsPort;
    yarp::os::BufferedPort<Vector> m_axisPort;
    yarp::os::BufferedPort<Vector> m_trackballPort;
    yarp::os::BufferedPort<Vector> m_touchPort;
    hatportType                    m_hatsPort;
    yarp::os::BufferedPort<Vector> m_stickPort;
    Vector                         m_buttons;
    Vector                         m_trackballs;
    Vector                         m_touch;
    Vector                         m_axis;
    yarp::sig::VectorOf<char>      m_hats;
    std::vector<Vector>            m_sticks;
    bool                           m_rpc_only;
    yarp::os::ConstString          m_local;
    yarp::os::ConstString          m_remoteRpc;
    int                            m_rate;
    yarp::os::Mutex                m_mutex;

    bool getCount(const int& vocab_toget, unsigned int& value);

public:
    JoypadControlClient():RateThread(DEFAULT_THREAD_PERIOD), m_rpc_only(false){}
    //rateThread
    virtual void run()   YARP_OVERRIDE;

    //DeviceDriver
    virtual bool open(yarp::os::Searchable& config)  YARP_OVERRIDE;
    virtual bool close() YARP_OVERRIDE;

    //IJoypadController;
    virtual bool getAxisCount(unsigned int& axis_count)YARP_OVERRIDE;

    virtual bool getButtonCount(unsigned int& button_count)YARP_OVERRIDE;

    virtual bool getTrackballCount(unsigned int& Trackball_count)YARP_OVERRIDE;

    virtual bool getHatCount(unsigned int& Hat_count)YARP_OVERRIDE;

    virtual bool getTouchSurfaceCount(unsigned int& touch_count)YARP_OVERRIDE;

    virtual bool getStickCount(unsigned int& stick_count)YARP_OVERRIDE;

    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF)YARP_OVERRIDE;

    virtual bool getButton(unsigned int button_id, float& value)YARP_OVERRIDE;

    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value)YARP_OVERRIDE;

    virtual bool getHat(unsigned int hat_id, unsigned char& value)YARP_OVERRIDE;

    virtual bool getAxis(unsigned int axis_id, double& value)YARP_OVERRIDE;

    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)YARP_OVERRIDE;

    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value)YARP_OVERRIDE;
};
