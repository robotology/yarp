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

#ifndef CONTROL_THREAD
#define CONTROL_THREAD

class ControlThread: public yarp::os::PeriodicThread
{
private:
    std::mutex            mtx;
    std::string           module_name;
    yarp::os::BufferedPort<yarp::os::Bottle>  port_command_out;
    yarp::os::BufferedPort<yarp::os::Bottle>  port_command_joints;
    double                start_time = 0.0;

    action_class          *current_action = nullptr;
    robotDriver           *current_driver=nullptr;
    action_status_enum    status = ACTION_IDLE;

    //if set to true, the system will halt if home position is halted.
    //otherwise it will continue after the timeout expires
    bool                  home_position_strict_check_enabled = false;
    double                home_position_tolerance = 2.0;
    double                home_position_timeout = 2.0;

public:
    bool                  enable_execute_joint_command=false;

    action_status_enum getStatus();
    bool action_stop();
    bool action_print();
    bool action_reset();
    bool action_forever();
    bool action_start();
    bool action_change(action_class *action, robotDriver *driver);
    bool action_getname(std::string& name);

    ControlThread(std::string name, double period = 0.005);
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
