/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>

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
    yarp::os::Semaphore mutex;
public:
    StageControl() : mutex(1) {
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
