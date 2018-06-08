/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Mutex.h>

#include <stage.h>

namespace yarp {
    namespace dev {
        class StageControl;
    }
}



class yarp::dev::StageControl : public DeviceDriver,
            public IVelocityControl, public yarp::os::Thread {
private:
    stg_world_t* world;
    stg_model_t* position;
    stg_model_t* laser;
    double setpoint[3];
    yarp::os::Mutex mutex;
public:
    StageControl() : mutex() {
        world = NULL;
        for (int i=0; i<3; i++) {
            setpoint[i] = 0;
        }
    }

    virtual bool getAxes(int *axes) {
        if (axes!=NULL) { *axes = 3; }
        return true;
    }

    virtual bool setVelocityMode() {
        return true;
    }

    virtual bool velocityMove(int j, double v) {
        setpoint[j] = v;
        return velocityMove(setpoint);
    }

    virtual bool velocityMove(const double *v);

    virtual bool setRefAcceleration(int j, double acc) {
        return false;
    }

    virtual bool setRefAccelerations(const double *accs) {
        return false;
    }

    virtual bool getRefAcceleration(int j, double *acc) {
        return false;
    }

    virtual bool getRefAccelerations(double *accs) {
        return false;
    }

    virtual bool stop(int j) {
        return false;
    }

    virtual bool stop() {
        return false;
    }


    virtual bool open(yarp::os::Searchable& config);

    virtual bool close();

    virtual void run();
};
