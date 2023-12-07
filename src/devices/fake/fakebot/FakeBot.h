/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>

YARP_DECLARE_LOG_COMPONENT(FAKEBOT)

/**
 * @ingroup dev_impl_fake dev_impl_deprecated
 *
 * \brief `fakebot` *deprecated*: Documentation to be added
 */
class FakeBot :
        public yarp::dev::DeprecatedDeviceDriver,
        public yarp::dev::IPositionControl,
        public yarp::dev::IVelocityControl,
        public yarp::dev::IAmplifierControl,
        public yarp::dev::IEncodersTimed,
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IControlCalibration,
        public yarp::dev::IControlLimits,
        public yarp::dev::DeviceResponder,
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
    FakeBot() :
        njoints(2),
        m_w(128),
        m_h(128),
        xScale(1),
        yScale(1),
        noiseLevel(0),
        lifetime(-1)
    {
        pos.resize(njoints);
        dpos.resize(njoints);
        vel.resize(njoints);
        speed.resize(njoints);
        acc.resize(njoints);
        loc.resize(njoints);
        amp.resize(njoints);
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
            dpos[i] = 0;
            vel[i] = 0;
            speed[i] = 0;
            acc[i] = 0;
            loc[i] = 0;
            amp[i] = 1; // initially on - ok for simulator
        }
        init();
    }

    bool open(yarp::os::Searchable& config) override;

    // IFrameGrabberImage
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    int height() const override{
        return m_h;
    }

    int width() const override{
        return m_w;
    }



    // IPositionControl etc.

    bool getAxes(int *ax) override {
        *ax = njoints;
        return true;
    }

    bool positionMove(int j, double ref) override {
        if (j<njoints) {
            pos[j] = ref;
        }
        return true;
    }


    bool positionMove(const double *refs) override {
        for (int i=0; i<njoints; i++) {
            pos[i] = refs[i];
        }
        return true;
    }


    bool relativeMove(int j, double delta) override {
        if (j<njoints) {
            dpos[j] = delta;
        }
        return true;
    }


    bool relativeMove(const double *deltas) override {
        for (int i=0; i<njoints; i++) {
            dpos[i] = deltas[i];
        }
        return true;
    }


    bool checkMotionDone(int j, bool *flag) override {
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


    bool close() override {
        return true;
    }

    bool resetEncoder(int j) override {
        if (j<njoints) {
            pos[j] = 0;
            dpos[j] = 0;
        }
        return true;
    }

    bool resetEncoders() override {
        for (int i=0; i<njoints; i++) {
            pos[i] = 0;
        }
        return true;
    }

    bool setEncoder(int j, double val) override {
        if (j<njoints) {
            pos[j] = val;
        }
        return true;
    }

    bool setEncoders(const double *vals) override {
        for (int i=0; i<njoints; i++) {
            pos[i] = vals[i];
        }
        return true;
    }

    bool getEncoder(int j, double *v) override {
        if (j<njoints) {
            (*v) = loc[j];
        }

        return true;
    }

    bool getEncoders(double *encs) override {
        for (int i=0; i<njoints; i++) {
            encs[i] = loc[i];
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

    bool positionMove(const int n_joint, const int *joints, const double *refs) override { return false; }

    bool relativeMove(const int n_joint, const int *joints, const double *deltas) override { return false; }

    bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override { return false; }

    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override { return false; }

    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override { return false; }

    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override { return false; }

    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override { return false; }

    bool stop(const int n_joint, const int *joints) override { return false; }


    // IEncodersTimed
    bool getEncodersTimed(double *encs, double *time) override
    {
        bool ret = getEncoders(encs);
        double myTime = yarp::os::Time::now();

        for (int i=0; i<njoints; i++)
        {
            time[i] = myTime;
        }
        return ret;
    }

    bool getEncoderTimed(int j, double *enc, double *time) override
    {
        bool ret = getEncoder(j, enc);
        *time = yarp::os::Time::now();
        return ret;
    }

    bool velocityMove(int j, double sp) override {
        if (j<njoints) {
            vel[j] = sp;
        }
        return true;
    }

    bool velocityMove(const double *sp) override {
        for (int i=0; i<njoints; i++) {
            vel[i] = sp[i];
        }
        return true;
    }

    bool velocityMove(const int n_joint, const int *joints, const double *spds) override { return false; }



    bool enableAmp(int j) override {
        if (j<njoints) {
            amp[j] = 1;
        }
        return true;
    }

    bool disableAmp(int j) override {
        if (j<njoints) {
            amp[j] = 0;
        }
        return true;
    }

    bool getCurrent(int j, double *val) override {
        if (j<njoints) {
            val[j] = amp[j];
        }
        return true;
    }

    bool getCurrents(double *vals) override {
        for (int i=0; i<njoints; i++) {
            vals[i] = amp[i];
        }
        return true;
    }

    bool getMaxCurrent(int j, double* v) override {
        *v = 0;
        return true;
    }

    bool setMaxCurrent(int j, double v) override {
        return true;
    }

    bool getAmpStatus(int *st) override {
        *st = 0;
        return true;
    }

    bool getAmpStatus(int k, int *v) override
    {
        *v=0;
        return true;
    }

    bool calibrateAxisWithParams(int j, unsigned int iv, double v1, double v2, double v3) override
    {
        yCWarning(FAKEBOT, "Calibrating joint %d with parameters %u %lf %lf %lf", j, iv, v1, v2, v3);
        return true;
    }

    bool calibrationDone(int j) override
    {
        yCWarning(FAKEBOT, "Calibration done on joint %d.", j);
        return true;
    }

    bool getLimits(int axis, double *min, double *max) override
    {
        yCWarning(FAKEBOT, "Get limits");
        *min=0;
        *max=0;
        return true;
    }

    bool setLimits(int axis, double min, double max) override
    {
        yCWarning(FAKEBOT, "Set limits");
        return true;
    }

    bool setVelLimits(int axis, double min, double max) override { return false; }

    bool getVelLimits(int axis, double *min, double *max) override { return false; }

    void run() override;
};
