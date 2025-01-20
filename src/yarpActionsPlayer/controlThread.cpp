/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "robotDriver.h"
#include "robotAction.h"
#include "controlThread.h"

action_status_enum ControlThread::getStatus()
{
    std::lock_guard<std::mutex> lg(mtx);
    return status;
}

bool ControlThread::action_getname(std::string& name)
{
    std::lock_guard<std::mutex> lg(mtx);
    name = current_action->action_name;
    return true;
}

bool ControlThread::action_stop()
{
    std::lock_guard<std::mutex> lg(mtx);
    status = ACTION_STOP;
    return true;
}

bool ControlThread::action_print()
{
    std::lock_guard<std::mutex> lg(mtx);
    current_action->print();
    return true;
}

bool ControlThread::action_reset()
{
    std::lock_guard<std::mutex> lg(mtx);
    status = ACTION_RESET;
    current_action->current_frame = 0;
    return true;
}

bool ControlThread::action_forever()
{
    std::lock_guard<std::mutex> lg(mtx);
    if (current_action->current_frame == 0)
        status = ACTION_START;
    else
        status = ACTION_RUNNING;
    current_action->forever = true;
    return true;
}

bool ControlThread::action_start()
{
    std::lock_guard<std::mutex> lg(mtx);
    if (current_action->current_frame == 0)
    {
        status = ACTION_START;
    }
    else
    {
        status = ACTION_RUNNING;
    }
    current_action->forever = false;
    return true;
}

bool ControlThread::action_change(action_class* action, robotDriver* driver)
{
    std::lock_guard<std::mutex> lg(mtx);
    current_action = action;
    current_driver = driver;
    status = ACTION_IDLE;
    current_action->current_frame = 0;
    return true;
}

ControlThread::ControlThread(std::string name, double period): PeriodicThread(period)
{
    module_name = name;
}

ControlThread::~ControlThread()
{
    port_command_out.interrupt();
    port_command_out.close();
    port_command_joints.interrupt();
    port_command_joints.close();
}

bool ControlThread::threadInit()
{
    if (!port_command_out.open(std::string("/")+module_name+"/port_command_out:o"))
    {
        return false;
    }

    if (!port_command_joints.open(std::string("/")+module_name+"/port_joints:o"))
    {
        return false;
    }
    return true;
}

bool ControlThread::execute_joint_command(int frame_id)
{
    if (!current_driver) return false;
    if (!enable_execute_joint_command) return true;

    double *ll = current_action->action_frames_vector[frame_id].q_joints.data();
    size_t nj = current_action->get_njoints();

    for (size_t j = 0; j < nj; j++)
    {
        current_driver->setPosition((int)j, ll[j]);
    }
    return true;
}

void ControlThread::compute_and_send_command(int frame_id)
{
    //prepare the output command
    yarp::os::Bottle& bot = port_command_out.prepare();
    bot.clear();
    bot.addInt32((int)current_action->action_frames_vector[frame_id].counter);
    bot.addFloat64(current_action->action_frames_vector[frame_id].time);

    //send the output command
    port_command_out.write();

    //execute the command
    if (!execute_joint_command(frame_id))
    {
        yError("failed to execute command");
    }

    //quick reads the current position
    std::vector<double> encs;
    if (current_driver)
    {
        size_t nj = current_action->get_njoints();
        encs.resize(nj);
        for (size_t j = 0; j < nj; j++)
        {
            current_driver->getEncoder((int)j, &encs[j]);
        }
    }
    else
    {
        //invalid driver
        yError("Critical error: invalid driver");
    }

    //send the joints angles on debug port
    double *ll = current_action->action_frames_vector[frame_id].q_joints.data();
    yarp::os::Bottle& bot2 = this->port_command_joints.prepare();
    bot2.clear();
    bot2.addInt32((int)current_action->action_frames_vector[frame_id].counter);
    bot2.addFloat64(current_action->action_frames_vector[frame_id].time);
    size_t nj = current_action->get_njoints();
    bot2.addString("commands:");
    for (size_t ix=0;ix<nj;ix++)
    {
        bot2.addFloat64(ll[ix]);
    }
    bot2.addString("encoders:");
    for (size_t ix=0;ix<nj;ix++)
    {
        bot2.addFloat64(encs[ix]);
    }
    this->port_command_joints.write();
}

void ControlThread::run()
{
    std::lock_guard<std::mutex> lck(mtx);

    double current_time = yarp::os::Time::now();
    size_t nj = current_action->get_njoints();

    if (this->status == ACTION_IDLE)
    {
        // do nothing
        //yDebug() << "ACTION_IDLE";
    }
    else if (this->status == ACTION_STOP)
    {
        yInfo() << "ACTION_STOP";
        this->status = ACTION_IDLE;
    }
    else if (this->status == ACTION_RESET)
    {
        yInfo() << "ACTION_RESET";

        for (size_t j = 0; j < nj; j++)
        {
            current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
        }
        this->status = ACTION_IDLE;
    }
    else if (this->status == ACTION_RUNNING)
    {
        size_t last_frame = current_action->action_frames_vector.size();
        if (last_frame == 0)
        {
            yError("ACTION_RUNNING: sequence empty!");
            this->status = ACTION_RESET;
            return;
        }

        //if it's not the last frame
        if (current_action->current_frame < last_frame - 1)
        {
            double elapsed_time = current_action->action_frames_vector[current_action->current_frame].time;
            if (current_time-start_time > elapsed_time)
            {
                current_action->current_frame++;
                compute_and_send_command((int)current_action->current_frame);
                yDebug("Executing action: %4zd/%4zd", current_action->current_frame , last_frame);
                //printf("EXECUTING %d, elapsed_time:%.5f requested_time:%.5f\n", actions.current_action, current_time-last_time, duration);
            }
            else
            {
                //printf("WAITING %d, elapsed_time:%.5f requested_time:%.5f\n", actions.current_action, current_time-last_time, duration);
            }
        }
        else //the action is complete
        {
            if (current_action->forever)
            {
                yInfo("sequence completed in: %f s, restarting", yarp::os::Time::now() - start_time);
                current_action->current_frame=0;
                start_time = yarp::os::Time::now();
            }
            else
            {
                yInfo("sequence completed in: %f s",yarp::os::Time::now()-start_time);
                for (size_t j = 0; j < nj; j++)
                {
                    current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
                }
                this->status=ACTION_IDLE;
            }
        }
    }
    else if (this->status == ACTION_START)
    {
        if (current_action->action_frames_vector.size() > 0)
        {
            double *ll = current_action->action_frames_vector[0].q_joints.data();
            size_t nj = current_action->get_njoints();

            //first go to first frame in standard position mode
            yDebug() << "ACTION_START: switch to position mode";
            for (size_t j = 0; j < nj; j++)
            {
                current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
            }
            yarp::os::Time::delay(0.1);
            for (size_t j = 0; j < nj; j++)
            {
                current_driver->positionMove((int)j, ll[j]);
            }

            //check if it reached the position within the desired tolerance
            yInfo() << "ACTION_START: going to start position";
            double enc= 0.0;
            bool check = true;
            double start_time = yarp::os::Time::now();
            do
            {
                for (size_t j = 0; j < nj; j++)
                {
                    current_driver->getEncoder((int)j, &enc);
                    double err = fabs(enc - ll[j]);
                    check = (err < home_position_tolerance);
                }
                yarp::os::Time::delay(0.1);
                if (check)
                {
                    yInfo() << "ACTION_START: start position reached successfully";
                    break;
                }
                if (yarp::os::Time::now() - start_time > home_position_timeout)
                {
                    yWarning() << "ACTION_START: timeout while trying to reach start position";
                    break;
                }
            } while (1);

            //switch to position direct mode
            if (check)
            {
                yDebug() << "ACTION_START: switch to position mode";

                for (int j = 0; j <nj; j++)
                {
                    current_driver->setControlMode(j, VOCAB_CM_POSITION_DIRECT);
                }
                yarp::os::Time::delay(0.1);
                compute_and_send_command(0);

                this->status = ACTION_RUNNING;
                start_time = yarp::os::Time::now();
            }
            else
            {
                yError() << "ACTION_START: unable to reach start position!";
                if (home_position_strict_check_enabled)
                {
                    //very strict behavior! if your are controlling fingers, you will probably end here
                    this->status = ACTION_STOP;
                }
                else
                {
                    yDebug() << "ACTION_START: switch to position direct mode";
                    for (int j = 0; j <nj; j++)
                    {
                        current_driver->setControlMode(j, VOCAB_CM_POSITION_DIRECT);
                    }
                    yarp::os::Time::delay(0.1);
                    compute_and_send_command(0);

                    this->status = ACTION_RUNNING;
                    start_time = yarp::os::Time::now();
                }
            }
            yInfo() << "ACTION_START: sequence started";
        }
        else
        {
            yWarning("no sequence in memory");
            this->status = ACTION_STOP;
        }
    }
    else
    {
        yError() << "unknown current_status";
    }
}

void ControlThread::setPositionTolerance(double tolerance)
{
    if (tolerance > 0)
    {
        home_position_tolerance = tolerance;
    }
}

void ControlThread::setPositionTimeout(double timeout)
{
    if (timeout > 0)
    {
        home_position_timeout = timeout;
    }
}

void ControlThread::setPositionStrictCheck(bool enable)
{
    home_position_strict_check_enabled=enable;
}
