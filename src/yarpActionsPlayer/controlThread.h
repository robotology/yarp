/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Math.h>

#include <string>
#include <cmath>
#include <mutex>

#include "robotDriver.h"
#include "robotAction.h"
#include "controlClock.h"

#ifndef CONTROL_THREAD
#define CONTROL_THREAD

class ControlThread: public yarp::os::PeriodicThread
{
private:
    std::mutex            m_mtx;
    std::string           m_module_name;
    yarp::os::BufferedPort<yarp::os::Bottle>  m_port_command_out;
    yarp::os::BufferedPort<yarp::os::Bottle>  m_port_command_joints;

    action_class          *m_current_action = nullptr;
    robotDriver           *m_current_driver=nullptr;
    action_status_enum    m_status = ACTION_IDLE;

    //if set to true, the system will halt if home position is halted.
    //otherwise it will continue after the timeout expires
    bool                  m_home_position_strict_check_enabled = false;
    double                m_home_position_tolerance = 2.0;
    double                m_home_position_timeout = 2.0;

public:
    bool                  m_enable_execute_joint_command=false;
    ControlClock          m_clock;

    action_status_enum getStatus();
    bool action_stop();
    bool action_print();
    bool action_reset();
    bool action_forever();
    bool action_start();
    bool action_change(action_class *action, robotDriver *driver);
    bool action_getname(std::string& name);
    bool action_setSpeedFactor(double factor = 1);
    bool action_resample(double value);

    ControlThread(std::string name, double period);
    ~ControlThread();
    bool threadInit() override;
    void run() override;

    void setPositionTolerance(double tolerance);
    void setPositionTimeout(double timeloops);
    void setPositionStrictCheck(bool enable);

private:
    bool execute_joint_command(int frame_id);
    void compute_and_send_command(int frame_id);
};

#endif
