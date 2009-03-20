// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef _YARP2_TESTMOTOR_
#define _YARP2_TESTMOTOR_

#include <stdio.h>


#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class TestMotor;
    }
}

/**
 * @ingroup dev_impl_motor
 *
 * A fake motor control board for testing.
 * Implements the IPositionControl and IEncoders interfaces.
 */
class yarp::dev::TestMotor : public DeviceDriver, 
            public IPositionControl, public IEncoders {
private:
    int njoints;
    yarp::sig::Vector pos, speed, acc;
public:
    TestMotor() {
        njoints = 1;
    }

    virtual bool getAxes(int *ax) {
        *ax = njoints;
        printf("TestMotor reporting %d axes present\n", *ax);
        return true;
    }

    virtual bool open(yarp::os::Searchable& config) {
        njoints = config.check("axes",yarp::os::Value(4),"number of axes to pretend to have").asInt();
        pos.size(njoints);
        speed.size(njoints);
        acc.size(njoints);
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
            speed[i] = 0;
            acc[i] = 0;
        }
        return true;
    }

    virtual bool setPositionMode() {
        return true;
    }

    virtual bool positionMove(int j, double ref) {
        if (j<njoints) {
            pos[j] = ref;
        }
        return true;
    }


    virtual bool positionMove(const double *refs) {
        for (int i=0; i<njoints; i++) {
            pos[i] = refs[i];
        }
        return true;
    }


    virtual bool relativeMove(int j, double delta) {
        if (j<njoints) {
            pos[j] += delta;
        }
        return true;
    }


    virtual bool relativeMove(const double *deltas) {
        for (int i=0; i<njoints; i++) {
            pos[i] += deltas[i];
        }
        return true;
    }


    virtual bool checkMotionDone(int j, bool *flag) {
        Time::delay(80/1000.0);
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
        return true;
    }

    virtual bool setEncoder(int j, double val) {
        if (j<njoints) {
            pos[j] = val;
        }
        return true;
    }

    virtual bool setEncoders(const double *vals) {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        return true;
    }

    virtual bool getEncoder(int j, double *v) {
        if (j<njoints) {
            (*v) = pos[j];
        }
        return true;
    }

    virtual bool getEncoders(double *encs) {
        for (int i=0; i<njoints; i++) {
            encs[i] = pos[i];
        }
        return true;
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
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_motor test_motor

 A fake motor control board, see yarp::dev::TestMotor.

*/


#endif
