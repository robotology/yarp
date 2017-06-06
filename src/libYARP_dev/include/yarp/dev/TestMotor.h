/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_DEV_TESTMOTOR_H
#define YARP_DEV_TESTMOTOR_H

#include <cstdio>


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

    virtual bool getAxes(int *ax) YARP_OVERRIDE {
        *ax = njoints;
        yInfo("TestMotor reporting %d axes present", *ax);
        return true;
    }

    virtual bool open(yarp::os::Searchable& config) YARP_OVERRIDE {
        njoints = config.check("axes",yarp::os::Value(4),"number of axes to pretend to have").asInt();
        pos.resize(njoints);
        speed.resize(njoints);
        acc.resize(njoints);
        vel.resize(njoints);
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
            speed[i] = 0;
            acc[i] = 0;
            vel[i] = 0;
        }

        delay=config.check("delay",yarp::os::Value(0), "delay in each call for debugging purpose, in ms").asInt();
        return true;
    }


    virtual bool positionMove(int j, double ref) YARP_OVERRIDE {
        posMode = true;
        if (j<njoints) {
            pos[j] = ref;
        }
        return true;
    }


    virtual bool positionMove(const double *refs) YARP_OVERRIDE {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] = refs[i];
        }
        return true;
    }


    virtual bool relativeMove(int j, double delta) YARP_OVERRIDE {
        posMode = true;
        if (j<njoints) {
            pos[j] += delta;
        }
        return true;
    }


    virtual bool relativeMove(const double *deltas) YARP_OVERRIDE {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] += deltas[i];
        }
        return true;
    }


    virtual bool checkMotionDone(int j, bool *flag) YARP_OVERRIDE {
        yarp::os::Time::delay(delay/1000.0);
        return true;
    }


    virtual bool checkMotionDone(bool *flag) YARP_OVERRIDE {
        return true;
    }


    virtual bool setRefSpeed(int j, double sp) YARP_OVERRIDE {
        if (j<njoints) {
            speed[j] = sp;
        }
        return true;
    }


    virtual bool setRefSpeeds(const double *spds) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            speed[i] = spds[i];
        }
        return true;
    }


    virtual bool setRefAcceleration(int j, double acc) YARP_OVERRIDE {
        if (j<njoints) {
            this->acc[j] = acc;
        }
        return true;
    }


    virtual bool setRefAccelerations(const double *accs) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            acc[i] = accs[i];
        }
        return true;
    }


    virtual bool getRefSpeed(int j, double *ref) YARP_OVERRIDE {
        if (j<njoints) {
            (*ref) = speed[j];
        }
        return true;
    }


    virtual bool getRefSpeeds(double *spds) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            spds[i] = speed[i];
        }
        return true;
    }


    virtual bool getRefAcceleration(int j, double *acc) YARP_OVERRIDE {
        if (j<njoints) {
            (*acc) = this->acc[j];
        }
        return true;
    }


    virtual bool getRefAccelerations(double *accs) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            accs[i] = acc[i];
        }
        return true;
    }


    virtual bool stop(int j) YARP_OVERRIDE {
        return true;
    }


    virtual bool stop() YARP_OVERRIDE {
        return true;
    }


    virtual bool close() YARP_OVERRIDE {
        return true;
    }

    virtual bool resetEncoder(int j) YARP_OVERRIDE {
        if (j<njoints) {
            pos[j] = 0;
        }
        return true;
    }

    virtual bool resetEncoders() YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
        }
        last = yarp::os::Time::now();
        return true;
    }

    virtual bool setEncoder(int j, double val) YARP_OVERRIDE {
        if (j<njoints) {
            pos[j] = val;
        }
        last = yarp::os::Time::now(); // not quite right, but ok for test
        return true;
    }

    virtual bool setEncoders(const double *vals) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        last = yarp::os::Time::now();
        return true;
    }

    virtual bool getEncoder(int j, double *v) YARP_OVERRIDE {
        update();
        if (j<njoints) {
            (*v) = pos[j];
        }
        return true;
    }

    virtual bool getEncoders(double *encs) YARP_OVERRIDE {
        update();
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
        }
        return true;
    }


    virtual bool getEncoderTimed(int j, double *encs, double *time) YARP_OVERRIDE
    {
        bool ret = getEncoder(j, encs);
        *time = yarp::os::Time::now();
        return ret;
    }

    virtual bool getEncodersTimed(double *encs, double *time) YARP_OVERRIDE
    {
        bool ret = getEncoders(encs);
        *time = yarp::os::Time::now();
        return ret;
    }

    virtual bool getEncoderSpeed(int j, double *sp) YARP_OVERRIDE {
        if (j<njoints) {
            (*sp) = 0;
        }
        return true;
    }

    virtual bool getEncoderSpeeds(double *spds) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            spds[i] = 0;
        }
        return true;
    }

    virtual bool getEncoderAcceleration(int j, double *spds) YARP_OVERRIDE {
        if (j<njoints) {
            (*spds) = 0;
        }
        return true;
    }

    virtual bool getEncoderAccelerations(double *accs) YARP_OVERRIDE {
        for (int i=0; i<njoints; i++) {
            accs[i] = 0;
        }
        return true;
    }

    virtual bool velocityMove(int j, double sp) YARP_OVERRIDE {
        posMode = false;
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    virtual bool velocityMove(const double *sp) YARP_OVERRIDE {
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


#endif // YARP_DEV_TESTMOTOR_H
