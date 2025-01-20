/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <fstream>

using namespace std;
using namespace yarp::os;

#include "robotAction.h"

// ******************** ACTION FRAME
action_frame::action_frame()
{
}

action_frame::action_frame(const action_frame& as)
{
    counter = as.counter;
    time = as.time;
    q_joints=as.q_joints;
}

action_frame & action_frame::operator=(const action_frame & as)
{
    if (this == &as)
    {
        return *this;
    }
    counter = as.counter;
    time = as.time;
    q_joints=as.q_joints;
    return *this;
}

action_frame::~action_frame()
{
}

// ******************** ACTION CLASS
size_t action_class::get_njoints()
{
    if (action_frames_vector.size() == 0)
        return 0;
    return action_frames_vector[0].q_joints.size();
}

void action_class::clear()
{
    forever = false;
    current_frame = 0;
    action_frames_vector.clear();
}

action_class::action_class()
{
    clear();
}

void action_class::print()
{
    std::deque<action_frame>::iterator action_frames_it;
    for (action_frames_it=action_frames_vector.begin(); action_frames_it<action_frames_vector.end(); action_frames_it++)
    {
        string s = "(" +
             std::to_string(action_frames_it->counter) + " " +
             std::to_string(action_frames_it->time) + ")   ";
        for (int i = 0; i < action_frames_it->q_joints.size(); i++)
        {
            s += std::to_string(action_frames_it->q_joints[i]);
        }
        yInfo() << s;
    }
}


std::string getFileExtension(const std::string& filename)
{
    //Finds the last occurrence of the dot
    std::size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        //Returns the file extension (after the dot)
        return filename.substr(dotPos + 1);
    }
    //If the dot is not found, return an empty string
    return "";
}

bool action_class::openFile(string filename, size_t njoints, double timestep)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        yError() << "Cannot find file" << filename;
        return false;
    }

    std::string line;
    std::getline(file, line); //Removes the remaining part of the line

    size_t linecount = 0;
    size_t wallcount = 0;
    double walltime = 0;
    while (std::getline(file, line))
    {
        if (timestep == -1)
        {
            if(!parseCommandLineVarTime(line, njoints))
            {
                yError ("error parsing file, line %d\n", linecount++);
                return false;
            };
        }
        else
        {
            if (!parseCommandLineFixTime(line, njoints, wallcount, walltime))
            {
                yError("error parsing file, line %d\n", linecount++);
                return false;
            }
            walltime += timestep;
            wallcount++;
        }
    }

    file.close();
    return true;
}

//FORMAT FILE
//0.0 0.0 0.0 0.0
bool action_class::parseCommandLineFixTime(std::string command_line, size_t njoints, size_t wallCount, double wallTime)
{
    std::istringstream iss(command_line);

    action_frame tmp_frame;
    tmp_frame.counter = wallCount;
    tmp_frame.time    = wallTime;

    double value = 0.0;
    std::vector<double> tempElements;
    while (iss >> value)
    {
         tempElements.push_back(value);
    }

    if (tempElements.size() != njoints)
    {
        yError("Invalid number of elements");
        return false;
    }
    for (size_t i = 0; i < njoints;i++)
    {
        tmp_frame.q_joints.push_back(tempElements[i]);
    }

    //insert the new frame in the list
    action_frames_vector.push_back(tmp_frame);

    return true;
}

//FORMAT FILE
//111 1.01222  0.0 0.0 0.0 0.0
bool action_class::parseCommandLineVarTime(std::string command_line, size_t njoints)
{
    std::istringstream iss(command_line);

    action_frame tmp_frame;

    iss >> tmp_frame.counter;
    iss >> tmp_frame.time;

    double value = 0.0;
    std::vector<double> tempElements;
    while (iss >> value)
    {
         tempElements.push_back(value);
    }

    if (tempElements.size() != njoints)
    {
        yError("Invalid number of elements");
        return false;
    }
    for (size_t i = 0; i << njoints;i++)
    {
        tmp_frame.q_joints[i]= tempElements[i];
    }

    //insertion of the new parsed frame in the vector of frames based on the timestamp
    std::deque<action_frame>::iterator action_frames_it;
    for (action_frames_it = action_frames_vector.begin(); action_frames_it < action_frames_vector.end(); action_frames_it++)
    {
        if (tmp_frame.time > action_frames_it->time)
        {
            break;
        }
        action_frames_vector.insert(action_frames_it, tmp_frame);
    }
    return true;
}
