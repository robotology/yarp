/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Vector.h>

#include <string>
#include <vector>
#include <deque>

#ifndef ROBOT_ACTION_H
#define ROBOT_ACTION_H

enum action_status_enum
{
    ACTION_IDLE   = 0,
    ACTION_START  = 1,
    ACTION_RUNNING= 2,
    ACTION_STOP   = 3,
    ACTION_RESET  = 4
};

// ******************** ACTION_FRAME CLASS
// This is a frame of an action.
// it contains: a counter, a time, a list of encoder values and a tag.
class action_frame
{
public:
    size_t      counter=0;
    double      time=0.0;
    std::vector<double> q_joints;

public:
    action_frame();
    action_frame(const action_frame& as);
    action_frame & operator=(const action_frame & as);
    ~action_frame();
};

// ******************** ACTION CLASS
// This is an action. It has a name and a sequence of action frames.
class action_class
{
public:
    std::string    action_name;
    std::string    controller_name;
    size_t         current_frame;
    bool           forever;
    double         speed_factor = 1.0;
    std::vector<double>      m_tolerances;
    std::deque<action_frame> action_frames_vector;

    action_class();

    void clear();
    void print();
    bool openFile(std::string filename, size_t njoints, double timestep=-1);
    size_t get_njoints();

    //Re-interpolates the action frames with the given constant time step
    void interpolate_action_frames(double timestep);

private:
    //internal use. Called by openFile
    bool parseCommandLineFixTime(std::string command_line, size_t n_joints, size_t wallcount, double wallTime);
    bool parseCommandLineVarTime(std::string command_line, size_t n_joints);

    //At time t1, joints have position q1. At time t2>t1, joints have position q2.
    //This function computes the value of joints q, at time t1<t<t2.
    std::vector<double> interpolate_joints(const std::vector<double>& q1, const std::vector<double>& q2, double t1, double t2, double t);
};

#endif
