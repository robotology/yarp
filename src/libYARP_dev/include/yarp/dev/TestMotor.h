// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_TESTMOTOR_
#define _YARP2_TESTMOTOR_

#include <stdio.h>


#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
        class TestMotor;
    }
}

/**
 * @ingroup dev_impl_motor
 *
 * A fake motor control board for testing.
 * Implements the IPositionControl, IEncoders and IVelocityControl interfaces.
 */
class YARP_dev_API yarp::dev::TestMotor : public DeviceDriver, 
                                          public IPositionControl, 
                                          public IEncodersTimed,
                                          public IVelocityControl {
private:
    int njoints;
    yarp::sig::Vector pos, speed, vel, acc;
    double delay;
    double last;
    bool posMode;

    void update() {
        if (posMode) return;
        double now = yarp::os::Time::now();
        if (last<0) last = now;
        double dt = now-last;
        for (int i=0; i<njoints; i++) {
            pos[i] += vel[i]*dt;
        }
        last = now;
    }

public:
    TestMotor() {
        njoints = 1;
        delay = 0;
        posMode = true;
        last = -1;
    }

    virtual bool getAxes(int *ax) {
        *ax = njoints;
        yInfo("TestMotor reporting %d axes present", *ax);
        return true;
    }

    virtual bool open(yarp::os::Searchable& config) {
        njoints = config.check("axes",yarp::os::Value(4),"number of axes to pretend to have").asInt();
        pos.size(njoints);
        speed.size(njoints);
        acc.size(njoints);
        vel.size(njoints);
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
            speed[i] = 0;
            acc[i] = 0;
            vel[i] = 0;
        }

        delay=config.check("delay",yarp::os::Value(0), "delay in each call for debugging purpose, in ms").asInt();
        return true;
    }

    virtual bool setPositionMode() {
        posMode = true;
        return true;
    }

    virtual bool positionMove(int j, double ref) {
        posMode = true;
        if (j<njoints) {
            pos[j] = ref;
        }
        return true;
    }


    virtual bool positionMove(const double *refs) {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] = refs[i];
        }
        return true;
    }


    virtual bool relativeMove(int j, double delta) {
        posMode = true;
        if (j<njoints) {
            pos[j] += delta;
        }
        return true;
    }


    virtual bool relativeMove(const double *deltas) {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] += deltas[i];
        }
        return true;
    }


    virtual bool checkMotionDone(int j, bool *flag) {
        yarp::os::Time::delay(delay/1000.0);
        return true;
    }


    virtual bool checkMotionDone(bool *flag) {
        return true;
    }


    virtual bool setRefSpeed(int j, double sp) {
        if (j<njoints) {
            speed[j] = sp;
        }
        return true;
    }


    virtual bool setRefSpeeds(const double *spds) {
        for (int i=0; i<njoints; i++) {
            speed[i] = spds[i];
        }
        return true;
    }


    virtual bool setRefAcceleration(int j, double acc) {
        if (j<njoints) {
            this->acc[j] = acc;
        }
        return true;
    }


    virtual bool setRefAccelerations(const double *accs) {
        for (int i=0; i<njoints; i++) {
            acc[i] = accs[i];
        }
        return true;
    }


    virtual bool getRefSpeed(int j, double *ref) {
        if (j<njoints) {
            (*ref) = speed[j];
        }
        return true;
    }


    virtual bool getRefSpeeds(double *spds) {
        for (int i=0; i<njoints; i++) {
            spds[i] = speed[i];
        }
        return true;
    }


    virtual bool getRefAcceleration(int j, double *acc) {
        if (j<njoints) {
            (*acc) = this->acc[j];
        }
        return true;
    }


    virtual bool getRefAccelerations(double *accs) {
        for (int i=0; i<njoints; i++) {
            accs[i] = acc[i];
        }
        return true;
    }


    virtual bool stop(int j) {
        return true;
    }


    virtual bool stop() {
        return true;
    }


    virtual bool close() {
        return true;
    }

    virtual bool resetEncoder(int j) {
        if (j<njoints) {
            pos[j] = 0;
        }
        return true;
    }

    virtual bool resetEncoders() {
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
        }
        last = yarp::os::Time::now();
        return true;
    }

    virtual bool setEncoder(int j, double val) {
        if (j<njoints) {
            pos[j] = val;
        }
        last = yarp::os::Time::now(); // not quite right, but ok for test
        return true;
    }

    virtual bool setEncoders(const double *vals) {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        last = yarp::os::Time::now();
        return true;
    }

    virtual bool getEncoder(int j, double *v) {
        update();
        if (j<njoints) {
            (*v) = pos[j];
        }
        return true;
    }

    virtual bool getEncoders(double *encs) {
        update();
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
        }
        return true;
    }


    virtual bool getEncoderTimed(int j, double *encs, double *time)
    {
        bool ret = getEncoder(j, encs);
        *time = yarp::os::Time::now();
        return ret;
    }

    virtual bool getEncodersTimed(double *encs, double *time)
    {
        bool ret = getEncoders(encs);
        *time = yarp::os::Time::now();
        return ret;
    }

    virtual bool getEncoderSpeed(int j, double *sp) {
        if (j<njoints) {
            (*sp) = 0;
        }
        return true;
    }

    virtual bool getEncoderSpeeds(double *spds) {
        for (int i=0; i<njoints; i++) {
            spds[i] = 0;
        }
        return true;
    }
    
    virtual bool getEncoderAcceleration(int j, double *spds) {
        if (j<njoints) {
            (*spds) = 0;
        }
        return true;
    }

    virtual bool getEncoderAccelerations(double *accs) {
        for (int i=0; i<njoints; i++) {
            accs[i] = 0;
        }
        return true;
    }

    virtual bool setVelocityMode() {
        posMode = false;
        return false;
    }


    virtual bool velocityMove(int j, double sp) {
        posMode = false;
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    virtual bool velocityMove(const double *sp) {
        posMode = false;
        for (int i=0; i<njoints; i++) {
            vel[i] = sp[i];
        }
        return true;
    }
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_motor test_motor

 A fake motor control board, see yarp::dev::TestMotor.

*/


#endif
