/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_TESTMOTOR_H
#define YARP_DEV_TESTMOTOR_H

#include <cstdio>


#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/sig/Vector.h>

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

    bool getAxes(int *ax) override {
        *ax = njoints;
        yInfo("TestMotor reporting %d axes present", *ax);
        return true;
    }

    bool open(yarp::os::Searchable& config) override {
        njoints = config.check("axes",yarp::os::Value(4),"number of axes to pretend to have").asInt32();
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

        delay=config.check("delay",yarp::os::Value(0), "delay in each call for debugging purpose, in ms").asInt32();
        return true;
    }


    bool positionMove(int j, double ref) override {
        posMode = true;
        if (j<njoints) {
            pos[j] = ref;
        }
        return true;
    }


    bool positionMove(const double *refs) override {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] = refs[i];
        }
        return true;
    }


    bool relativeMove(int j, double delta) override {
        posMode = true;
        if (j<njoints) {
            pos[j] += delta;
        }
        return true;
    }


    bool relativeMove(const double *deltas) override {
        posMode = true;
        for (int i=0; i<njoints; i++) {
            pos[i] += deltas[i];
        }
        return true;
    }


    bool checkMotionDone(int j, bool *flag) override {
        yarp::os::Time::delay(delay/1000.0);
        return true;
    }


    bool checkMotionDone(bool *flag) override {
        return true;
    }


    bool setRefSpeed(int j, double sp) override {
        if (j<njoints) {
            speed[j] = sp;
        }
        return true;
    }


    bool setRefSpeeds(const double *spds) override {
        for (int i=0; i<njoints; i++) {
            speed[i] = spds[i];
        }
        return true;
    }


    bool setRefAcceleration(int j, double acc) override {
        if (j<njoints) {
            this->acc[j] = acc;
        }
        return true;
    }


    bool setRefAccelerations(const double *accs) override {
        for (int i=0; i<njoints; i++) {
            acc[i] = accs[i];
        }
        return true;
    }


    bool getRefSpeed(int j, double *ref) override {
        if (j<njoints) {
            (*ref) = speed[j];
        }
        return true;
    }


    bool getRefSpeeds(double *spds) override {
        for (int i=0; i<njoints; i++) {
            spds[i] = speed[i];
        }
        return true;
    }


    bool getRefAcceleration(int j, double *acc) override {
        if (j<njoints) {
            (*acc) = this->acc[j];
        }
        return true;
    }


    bool getRefAccelerations(double *accs) override {
        for (int i=0; i<njoints; i++) {
            accs[i] = acc[i];
        }
        return true;
    }


    bool stop(int j) override {
        return true;
    }


    bool stop() override {
        return true;
    }

    bool positionMove(const int n_joint, const int *joints, const double *refs) override
    {
        return false;
    }

    bool relativeMove(const int n_joint, const int *joints, const double *deltas) override
    {
        return false;
    }

    bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override
    {
        return false;
    }

    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override
    {
        return false;
    }

    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override
    {
        return false;
    }

    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override
    {
        return false;
    }

    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override
    {
        return false;
    }

    bool stop(const int n_joint, const int *joints) override
    {
        return false;
    }

    bool getTargetPosition(const int joint, double *ref) override
    {
        return false;
    }

    bool getTargetPositions(double *refs) override
    {
        return false;
    }

    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override
    {
        return false;
    }

    bool velocityMove(const int n_joint, const int *joints, const double *spds) override
    {
        return false;
    }

    bool close() override {
        return true;
    }

    bool resetEncoder(int j) override {
        if (j<njoints) {
            pos[j] = 0;
        }
        return true;
    }

    bool resetEncoders() override {
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
        }
        last = yarp::os::Time::now();
        return true;
    }

    bool setEncoder(int j, double val) override {
        if (j<njoints) {
            pos[j] = val;
        }
        last = yarp::os::Time::now(); // not quite right, but ok for test
        return true;
    }

    bool setEncoders(const double *vals) override {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        last = yarp::os::Time::now();
        return true;
    }

    bool getEncoder(int j, double *v) override {
        update();
        if (j<njoints) {
            (*v) = pos[j];
        }
        return true;
    }

    bool getEncoders(double *encs) override {
        update();
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
        }
        return true;
    }


    bool getEncoderTimed(int j, double *encs, double *time) override
    {
        bool ret = getEncoder(j, encs);
        *time = yarp::os::Time::now();
        return ret;
    }

    bool getEncodersTimed(double *encs, double *time) override
    {
        for (int i=0; i<njoints; i++)
        {
            getEncoderTimed(i, &encs[i], &time[i]);
        }
        return true;
    }

    bool getEncoderSpeed(int j, double *sp) override {
        if (j<njoints) {
            (*sp) = 0;
        }
        return true;
    }

    bool getEncoderSpeeds(double *spds) override {
        for (int i=0; i<njoints; i++) {
            spds[i] = 0;
        }
        return true;
    }

    bool getEncoderAcceleration(int j, double *spds) override {
        if (j<njoints) {
            (*spds) = 0;
        }
        return true;
    }

    bool getEncoderAccelerations(double *accs) override {
        for (int i=0; i<njoints; i++) {
            accs[i] = 0;
        }
        return true;
    }

    bool velocityMove(int j, double sp) override {
        posMode = false;
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    bool velocityMove(const double *sp) override {
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
