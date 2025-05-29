/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <string>

#ifndef ROBOTDRIVER_H
#define ROBOTDRIVER_H

// ******************** ROBOT DRIVER CLASS
class robotDriver
{
    friend class BroadcastingThread;
private:
    int               n_joints=0;
    bool              verbose=true;
    yarp::os::Property          drvOptions_ll;
    yarp::dev::PolyDriver        drv_ll;
    yarp::dev::IPositionControl *ipos_ll=nullptr;
    yarp::dev::IPositionDirect  *iposdir_ll=nullptr;
    yarp::dev::IPidControl      *ipid_ll=nullptr;
    yarp::dev::IControlMode     *icmd_ll=nullptr;
    yarp::dev::IEncoders        *ienc_ll=nullptr;
    yarp::dev::IMotorEncoders   *imotenc_ll=nullptr;

public:
    robotDriver();
    bool configure(const yarp::os::Property &copt);
    bool init();
    ~robotDriver();
    size_t getNJoints();
    bool setControlMode(const int j, const int mode);
    bool setPosition(int j, double ref);
    bool getEncoder(int j, double *v);
    bool positionMove(int j, double ref);
};

#endif
