/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <vector>
#include <JoypadControlNetUtils.h>

#define DEFAULT_THREAD_PERIOD 10

class JoypadControlWatchdog :
        public yarp::os::PeriodicThread
{
public:
    JoypadControlWatchdog() : PeriodicThread(0.250) {}
    virtual ~JoypadControlWatchdog() = default;


    std::vector<JoypadControl::LoopablePort*> m_ports;
    void run() override;

};

/**
* @ingroup dev_impl_network_clients
*
* \brief `JoypadControlClient`: joypad input network wrapper on client side
*
* \section JoypadControlClient Description of input parameters
*
* Parameters accepted in the config argument of the open method:
* |   Parameter name  | Type   | Units | Default Value | Required  | Description                                  | Notes |
* |:-----------------:|:------:|:-----:|:-------------:|:---------:|:--------------------------------------------:|:-----:|
* | local             | string |       |               | yes       | name for the local port to open              |       |
* | remote            | string |       |               | yes       | name of the remote server port to connect to |       |
**/
class JoypadControlClient :
        public yarp::dev::IJoypadEventDriven,
        public yarp::dev::DeviceDriver
{
private:
    //---------------utils
    template<typename T>
    using JOYPORT = JoypadControl::JoyPort<T>;
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
    std::vector<size_t>   m_stickDof;
    bool                  m_rpc_only;
    std::string m_local;
    std::string m_remote;

    JoypadControlWatchdog          watchdog;
    std::vector<JoypadControl::LoopablePort*> m_ports;

    //--------------method
    bool getCount(const int& vocab_toget, unsigned int& value);
    bool getJoypadInfo();

public:
    JoypadControlClient();
    JoypadControlClient(const JoypadControlClient&) = delete;
    JoypadControlClient(JoypadControlClient&&) = delete;
    JoypadControlClient& operator=(const JoypadControlClient&) = delete;
    JoypadControlClient& operator=(JoypadControlClient&&) = delete;
    ~JoypadControlClient() override = default;

    //rateThread

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IJoypadController;
    bool getRawAxisCount(unsigned int& axis_count) override;
    bool getRawButtonCount(unsigned int& button_count) override;
    bool getRawTrackballCount(unsigned int& Trackball_count) override;
    bool getRawHatCount(unsigned int& Hat_count) override;
    bool getRawTouchSurfaceCount(unsigned int& touch_count) override;
    bool getRawStickCount(unsigned int& stick_count) override;
    bool getRawStickDoF(unsigned int stick_id, unsigned int& DoF) override;
    bool getRawButton(unsigned int button_id, float& value) override;
    bool getRawTrackball(unsigned int trackball_id, yarp::sig::Vector& value) override;
    bool getRawHat(unsigned int hat_id, unsigned char& value) override;
    bool getRawAxis(unsigned int axis_id, double& value) override;
    bool getRawStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) override;
    bool getRawTouch(unsigned int touch_id, yarp::sig::Vector& value) override;
};
