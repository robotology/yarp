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
    std::lock_guard<std::mutex> lg(m_mtx);
    return m_status;
}

bool ControlThread::action_getname(std::string& name)
{
    std::lock_guard<std::mutex> lg(m_mtx);
    name = m_current_action->action_name;
    return true;
}

bool ControlThread::action_stop()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_status = ACTION_STOP;
    m_clock.pauseTimer();
    return true;
}

bool ControlThread::action_print()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_current_action->print();
    return true;
}

bool ControlThread::action_reset()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_status = ACTION_RESET;
    m_current_action->current_frame = 0;
    m_clock.resetTimer();
    return true;
}

bool ControlThread::action_forever()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    if (m_current_action->current_frame == 0)
    {
        m_status = ACTION_START;
    }
    else
    {
        m_status = ACTION_RUNNING;
    }
    m_current_action->forever = true;
    return true;
}

bool ControlThread::action_start()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    if (m_current_action->current_frame == 0)
    {
        m_status = ACTION_START;
    }
    else
    {
        m_status = ACTION_RUNNING;
    }
    m_current_action->forever = false;
    return true;
}

bool ControlThread::action_change(action_class* action, robotDriver* driver)
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_current_action = action;
    m_current_driver = driver;
    m_status = ACTION_IDLE;
    m_current_action->current_frame = 0;
    return true;
}

bool ControlThread::action_setSpeedFactor(double factor)
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_current_action->speed_factor = factor;
    //also reset the action to avoid issue if it is playing..
    m_status = ACTION_RESET;
    m_current_action->current_frame = 0;
    return true;
}

bool ControlThread::action_resample(double value)
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_current_action->interpolate_action_frames(value);

    //also reset the action to avoid issue if it is playing..
    m_status = ACTION_RESET;
    m_current_action->current_frame = 0;
    return true;
}

ControlThread::ControlThread(std::string name, double period): PeriodicThread(period)
{
    yInfo() << "Control thread period set to: " << period;
    m_module_name = name;
}

ControlThread::~ControlThread()
{
    m_port_command_out.interrupt();
    m_port_command_out.close();
    m_port_command_joints.interrupt();
    m_port_command_joints.close();
}

bool ControlThread::threadInit()
{
    if (!m_port_command_out.open(std::string("/")+m_module_name+"/port_command_out:o"))
    {
        return false;
    }

    if (!m_port_command_joints.open(std::string("/")+m_module_name+"/port_joints:o"))
    {
        return false;
    }
    return true;
}

bool ControlThread::execute_joint_command(int frame_id)
{
    if (!m_current_driver) return false;
    if (!m_enable_execute_joint_command) return true;

    double *ll = m_current_action->action_frames_vector[frame_id].q_joints.data();
    size_t nj = m_current_action->get_njoints();

    for (size_t j = 0; j < nj; j++)
    {
        m_current_driver->setPosition((int)j, ll[j]);
    }
    return true;
}

void ControlThread::compute_and_send_command(int frame_id)
{
    //prepare the output command
    yarp::os::Bottle& bot = m_port_command_out.prepare();
    bot.clear();
    bot.addInt32((int)m_current_action->action_frames_vector[frame_id].counter);
    bot.addFloat64(m_current_action->action_frames_vector[frame_id].time);

    //send the output command
    m_port_command_out.write();

    //execute the command
    if (!execute_joint_command(frame_id))
    {
        yError("failed to execute command");
    }

    //quick reads the current position
    std::vector<double> encs;
    if (m_current_driver)
    {
        size_t nj = m_current_action->get_njoints();
        encs.resize(nj);
        for (size_t j = 0; j < nj; j++)
        {
            m_current_driver->getEncoder((int)j, &encs[j]);
        }
    }
    else
    {
        //invalid driver
        yError("Critical error: invalid driver");
    }

    //send the joints angles on debug port
    double *ll = m_current_action->action_frames_vector[frame_id].q_joints.data();
    yarp::os::Bottle& bot2 = this->m_port_command_joints.prepare();
    bot2.clear();
    bot2.addInt32((int)m_current_action->action_frames_vector[frame_id].counter);
    bot2.addFloat64(m_current_action->action_frames_vector[frame_id].time);
    size_t nj = m_current_action->get_njoints();
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
    this->m_port_command_joints.write();
}

size_t getCurrentFrame(double elapsed, std::deque<action_frame>& dq)
{
    //example
    // 0  0.0 > 1.1 ? no, continue
    // 1  0.5 > 1.1 ? no, continue
    // 2  1.0 > 1.1 ? no, continue
    // 3  1.5 > 1.1 ? yes, return 3
    // 4  2.0

    size_t i = 0;
    for (i = 0; i < dq.size(); i++)
    {
        if (dq[i].time > elapsed)
            return i;
    }
    return i;
}

void ControlThread::run()
{
    std::lock_guard<std::mutex> lck(m_mtx);

    size_t nj = m_current_action->get_njoints();

    if (this->m_status == ACTION_IDLE)
    {
        // do nothing
        //yDebug() << "ACTION_IDLE";
    }
    else if (this->m_status == ACTION_STOP)
    {
        yInfo() << "ACTION_STOP";
        this->m_status = ACTION_IDLE;
    }
    else if (this->m_status == ACTION_RESET)
    {
        yInfo() << "ACTION_RESET";

        for (size_t j = 0; j < nj; j++)
        {
            m_current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
        }
        this->m_status = ACTION_IDLE;
    }
    else if (this->m_status == ACTION_RUNNING)
    {
        size_t last_frame = m_current_action->action_frames_vector.size();
        if (last_frame == 0)
        {
            yError("ACTION_RUNNING: sequence empty!");
            this->m_status = ACTION_RESET;
            return;
        }

        //if it's not the last frame
        double curr_frame_time = m_clock.getElapsedTime() * m_current_action->speed_factor;
        size_t newcurrframe = getCurrentFrame(curr_frame_time, m_current_action->action_frames_vector);
        m_current_action->current_frame = newcurrframe;

        // not the last frame
        if (m_current_action->current_frame < last_frame - 1)
        {
            compute_and_send_command((int)m_current_action->current_frame);
            yDebug("Executing action: %4zd/%4zd (%.3fs)", m_current_action->current_frame , last_frame, m_clock.getElapsedTime());
        }
        else //the action is complete
        {
            if (m_current_action->forever)
            {
                yInfo("sequence completed in: %.3f s, restarting", m_clock.getElapsedTime());
                m_current_action->current_frame=0;
                m_clock.resetTimer();
                m_clock.startTimer();
            }
            else
            {
                yInfo("sequence completed in: %.3f s", m_clock.getElapsedTime());
                for (size_t j = 0; j < nj; j++)
                {
                    m_current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
                }
                this->m_status=ACTION_IDLE;
            }
        }
    }
    else if (this->m_status == ACTION_START)
    {
        if (m_current_action->action_frames_vector.size() > 0)
        {
            double *ll = m_current_action->action_frames_vector[0].q_joints.data();
            size_t nj = m_current_action->get_njoints();

            //first go to first frame in standard position mode
            yDebug() << "ACTION_START: switch to position mode";
            for (size_t j = 0; j < nj; j++)
            {
                m_current_driver->setControlMode((int)j, VOCAB_CM_POSITION);
            }
            yarp::os::Time::delay(0.1);
            for (size_t j = 0; j < nj; j++)
            {
                m_current_driver->positionMove((int)j, ll[j]);
            }

            //check if it reached the position within the desired tolerance
            yInfo() << "ACTION_START: going to start position";
            double enc= 0.0;
            bool check = true;
            double move_start_time = yarp::os::Time::now();
            do
            {
                for (size_t j = 0; j < nj; j++)
                {
                    m_current_driver->getEncoder((int)j, &enc);
                    double err = fabs(enc - ll[j]);
                    check = (err < m_home_position_tolerance);
                }
                yarp::os::Time::delay(0.1);
                if (check)
                {
                    yInfo() << "ACTION_START: start position reached successfully";
                    break;
                }
                if (yarp::os::Time::now() - move_start_time > m_home_position_timeout)
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
                    m_current_driver->setControlMode(j, VOCAB_CM_POSITION_DIRECT);
                }
                yarp::os::Time::delay(0.1);
                compute_and_send_command(0);

                this->m_status = ACTION_RUNNING;
            }
            else
            {
                yError() << "ACTION_START: unable to reach start position!";
                if (m_home_position_strict_check_enabled)
                {
                    //very strict behavior! if your are controlling fingers, you will probably end here
                    this->m_status = ACTION_STOP;
                }
                else
                {
                    yDebug() << "ACTION_START: switch to position direct mode";
                    for (int j = 0; j <nj; j++)
                    {
                        m_current_driver->setControlMode(j, VOCAB_CM_POSITION_DIRECT);
                    }
                    yarp::os::Time::delay(0.1);
                    compute_and_send_command(0);

                    this->m_status = ACTION_RUNNING;
                }
            }
            m_clock.startTimer();
            yInfo() << "ACTION_START: sequence started";
        }
        else
        {
            yWarning("no sequence in memory");
            m_clock.resetTimer();
            this->m_status = ACTION_STOP;
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
        m_home_position_tolerance = tolerance;
    }
}

void ControlThread::setPositionTimeout(double timeout)
{
    if (timeout > 0)
    {
        m_home_position_timeout = timeout;
    }
}

void ControlThread::setPositionStrictCheck(bool enable)
{
    m_home_position_strict_check_enabled=enable;
}
