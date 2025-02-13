/*
 * SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IPidControl.h>

#include "robotDriver.h"
#include "robotAction.h"
#include "broadcastingThread.h"


BroadcastingThread::BroadcastingThread(std::string name, robotDriver *p, action_class *a, double period): PeriodicThread(period)
{
    yAssert(p != nullptr);
    yAssert(a != nullptr);

    module_name = name;
    driver = p;
    actions = a;
}

BroadcastingThread::~BroadcastingThread()
{
    port_data_out.interrupt();
    port_data_out.close();
}

bool BroadcastingThread::threadInit()
{
    if (!port_data_out.open(std::string("/") + module_name + "/all_joints_data_out:o"));
    {
        return false;
    }

    if (!driver)
    {
        return false;
    }

    if (!actions)
    {
        return false;
    }

    njoints = driver->getNJoints();
    encs.resize(njoints);
    outs.resize(njoints);
    errs.resize(njoints);
    mots.resize(njoints);

    return true;
}

void BroadcastingThread::run()
{
    //reads the current position
    if (driver && driver->ienc_ll)
    {
        driver->ienc_ll->getEncoders(encs.data());
    }
    else
    {
        //invalid driver
    }

    //reads the pid output
    if (driver && driver->ipid_ll)
    {
        driver->ipid_ll->getPidOutputs(yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION,outs.data());
    }
    else
    {
        //invalid driver
    }

    //reads the pid error
    if (driver && driver->ipid_ll)
    {
        driver->ipid_ll->getPidErrors(yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION,errs.data());
    }
    else
    {
        //invalid driver
    }

    //reads the motor encoders
    if (driver && driver->imotenc_ll)
    {
        driver->imotenc_ll->getMotorEncoders(mots.data());
    }
    else
    {
        //invalid driver
    }

    size_t j = actions->current_frame;

    yarp::os::Bottle& bot2 = this->port_data_out.prepare();
    bot2.clear();
    bot2.addInt32((int)actions->action_frames_vector[j].counter);
    bot2.addFloat64(actions->action_frames_vector[j].time);

    size_t size = this->actions->action_frames_vector[j].q_joints.size();
    double *ll = actions->action_frames_vector[j].q_joints.data();

    bot2.addString("commands:");
    for (int ix=0;ix<size;ix++)
    {
        bot2.addFloat64(ll[ix]);
    }
    bot2.addString("joint encoders:");
    for (int ix=0;ix<size;ix++)
    {
        bot2.addFloat64(encs[ix]);
    }
    bot2.addString("outputs:");
    for (int ix=0;ix<size;ix++)
    {
        bot2.addFloat64(outs[ix]);
    }
    bot2.addString("motor encoders:");
    for (int ix=0;ix<size;ix++)
    {
        bot2.addFloat64(mots[ix]);
    }
    bot2.addString("errors:");
    for (int ix=0;ix<size;ix++)
    {
        bot2.addFloat64(errs[ix]);
    }
    bot2.addString("timestamp:");
    bot2.addFloat64(yarp::os::Time::now());
    this->port_data_out.write();
}
