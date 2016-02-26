/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/DeviceDriver.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Time.h>

namespace yarp {
    namespace dev {
      class FakeBot;
    }
}

class yarp::dev::FakeBot : public DeviceDriver,
            public IPositionControl,
            public IVelocityControl,
            public IAmplifierControl,
            public IEncodersTimed,
            public IFrameGrabberImage,
            public IControlCalibration2,
            public IControlLimits,
            public DeviceResponder,
            public yarp::os::Thread
{
private:
    int njoints;
    double m_x, m_y;
    double m_dx, m_dy;
    double m_tx, m_ty;
    double m_tdx, m_tdy;
    int m_w, m_h;
    double xScale, yScale;
    double noiseLevel;
    yarp::sig::Vector pos, dpos, vel, speed, acc, loc, amp;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> back, fore;
    double lifetime;

    void init();
public:
    FakeBot() {
        njoints = 2;
        m_w = 128;
        m_h = 128;
        pos.size(njoints);
        dpos.size(njoints);
        vel.size(njoints);
        speed.size(njoints);
        acc.size(njoints);
        loc.size(njoints);
        amp.size(njoints);
        xScale = 1;
        yScale = 1;
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
            dpos[i] = 0;
            vel[i] = 0;
            speed[i] = 0;
            acc[i] = 0;
            loc[i] = 0;
            amp[i] = 1; // initially on - ok for simulator
        }
        lifetime = -1;
        init();
    }

    virtual bool open(yarp::os::Searchable& config);

    // IFrameGrabberImage
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    virtual int height() const {
        return m_h;
    }

    virtual int width() const {
        return m_w;
    }



    // IPositionControl etc.

    virtual bool getAxes(int *ax) {
        *ax = njoints;
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
            dpos[j] = delta;
        }
        return true;
    }


    virtual bool relativeMove(const double *deltas) {
        for (int i=0; i<njoints; i++) {
            dpos[i] = deltas[i];
        }
        return true;
    }


    virtual bool checkMotionDone(int j, bool *flag) {
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
            dpos[j] = 0;
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
            (*v) = loc[j];
        }

        return true;
    }

    virtual bool getEncoders(double *encs) {
        for (int i=0; i<njoints; i++) {
            encs[i] = loc[i];
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


    // IEncodersTimed
    virtual bool getEncodersTimed(double *encs, double *time)
    {
        bool ret = getEncoders(encs);
        double myTime = yarp::os::Time::now();

        for (int i=0; i<njoints; i++)
        {
            time[i] = myTime;
        }
        return ret;
    }

    virtual bool getEncoderTimed(int j, double *enc, double *time)
    {
        bool ret = getEncoder(j, enc);
        *time = yarp::os::Time::now();
        return ret;
    }


    virtual bool setVelocityMode() {
        return true;
    }

    virtual bool velocityMove(int j, double sp) {
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    virtual bool velocityMove(const double *sp) {
        for (int i=0; i<njoints; i++) {
            vel[i] = sp[i];
        }
        return true;
    }



    virtual bool enableAmp(int j) {
        if (j<njoints) {
            amp[j] = 1;
        }
        return true;
    }

    virtual bool disableAmp(int j) {
        if (j<njoints) {
            amp[j] = 0;
        }
        return true;
    }

    virtual bool getCurrent(int j, double *val) {
        if (j<njoints) {
            val[j] = amp[j];
        }
        return true;
    }

    virtual bool getCurrents(double *vals) {
        for (int i=0; i<njoints; i++) {
            vals[i] = amp[i];
        }
        return true;
    }

    virtual bool getMaxCurrent(int j, double* v) {
        *v = 0;
        return true;
    }
    
    virtual bool setMaxCurrent(int j, double v) {
        return true;
    }

    virtual bool getAmpStatus(int *st) {
        *st = 0;
        return true;
    }

    virtual bool getAmpStatus(int k, int *v)
    {
        *v=0;
        return true;
    }

    virtual bool calibrate2(int j, unsigned int iv, double v1, double v2, double v3)
    {
        fprintf(stderr, "FakeBot: calibrating joint %d with parameters %u %lf %lf %lf\n", j, iv, v1, v2, v3);
        return true;
    }

    virtual bool done(int j)
    {
        fprintf(stderr , "FakeBot: calibration done on joint %d.\n", j);
        return true;
    }

    virtual bool getLimits(int axis, double *min, double *max)
    {
        fprintf(stderr, "FakeBot: get limits\n");
        *min=0;
        *max=0;
        return true;
    }

    virtual bool setLimits(int axis, double min, double max)
    {
        fprintf(stderr, "FakeBot: set limits\n");
        return true;
    }

    virtual void run();
};

