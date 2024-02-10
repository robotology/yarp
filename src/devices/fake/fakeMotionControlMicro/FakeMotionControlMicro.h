/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICE_FAKE_MOTIONCONTROLMICRO
#define YARP_DEVICE_FAKE_MOTIONCONTROLMICRO

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ImplementJointFault.h>

#include <mutex>
#include "FakeMotionControlMicro_ParamsParser.h"

/**
 * @ingroup dev_impl_fake dev_impl_motor
 *
 * \brief `fakeMotionControlMicro`: This device implements a minimal subset of mandatory interfaces
 * to run with controlBoard_nws_yarp. It is thus a smaller, limited version than fakeMotionControl.
 *
 */
class FakeMotionControlMicro :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IEncodersTimedRaw,
        public yarp::dev::IMotorEncodersRaw,
        public yarp::dev::IAxisInfoRaw,
        public yarp::dev::IJointFaultRaw,
        public yarp::dev::ImplementJointFault,
        public yarp::dev::ImplementAxisInfo,
        public yarp::dev::ImplementEncodersTimed,
        public yarp::dev::ImplementMotorEncoders,
        public FakeMotionControlMicro_ParamsParser
{
private:
    enum VerboseLevel
    {
        MUTE                = 0,    // only errors that prevent device from working
        QUIET               = 1,    // adds errors that can cause malfunctioning
        DEFAULT             = 2,    // adds warnings // DEFAULT // show noisy messages about back-compatible changes
        CHATTY              = 3,    // adds info messages
        VERBOSE             = 4,    // adds debug messages
        VERY_VERBOSE        = 5,    // adds trace of events (shows thread running and catch if they get stuck)
        VERY_VERY_VERBOSE   = 6     // adds messages printed every cycle, so too much verbose for usage, only for deep debugging
    };

    std::recursive_mutex _mutex;
    int  _njoints;
    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to iCubDegrees conversion factors */
    double  *_encodersStamp;                    /** keep information about acquisition time for encoders read */

    std::string *_axisName;                     /** axis name */
    yarp::dev::JointTypeEnum *_jointType;       /** axis type */

    bool        verbosewhenok;
    bool        useRawEncoderData;


    // internal stuff
    int     *_controlModes = nullptr;
    int     *_hwfault_code = nullptr;
    std::string  *_hwfault_message = nullptr;
    yarp::sig::Vector pos, dpos, vel, speed, acc, loc, amp;
    double prev_time;
    bool opened;

    // debugging
    VerboseLevel verbose;
public:

    FakeMotionControlMicro();
    ~FakeMotionControlMicro();

  // Device Driver
    bool open(yarp::os::Searchable &par) override;
    bool close() override;
    bool fromConfig(yarp::os::Searchable &config);

    virtual bool initialised();

    /**
     * Allocated buffers.
     */
    bool alloc(int njoints);

    /**
     * Resize previously allocated buffers.
     */
    void resizeBuffers();

    bool threadInit() override;
    void threadRelease() override;

    // IJointFault
    bool getLastJointFaultRaw(int j, int& fault, std::string& message) override;

    //////////////////////// BEGIN MotorEncoder Interface
    bool getNumberOfMotorEncodersRaw(int* num) override;
    bool resetMotorEncoderRaw(int j) override;
    bool resetMotorEncodersRaw() override;
    bool setMotorEncoderRaw(int j, double val) override;
    bool setMotorEncodersRaw(const double* vals) override;
    bool getMotorEncoderRaw(int j, double* v) override;
    bool getMotorEncodersRaw(double* encs) override;
    bool getMotorEncoderSpeedRaw(int j, double* sp) override;
    bool getMotorEncoderSpeedsRaw(double* spds) override;
    bool getMotorEncoderAccelerationRaw(int j, double* spds) override;
    bool getMotorEncoderAccelerationsRaw(double* accs) override;
    bool getMotorEncodersTimedRaw(double* encs, double* stamps) override;
    bool getMotorEncoderTimedRaw(int m, double* encs, double* stamp) override;
    bool getMotorEncoderCountsPerRevolutionRaw(int m, double* v) override;
    bool setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr) override;
    ///////////////////////// END MotorEncoder Interface

    //////////////////////// BEGIN EncoderInterface
    bool getAxes(int* ax) override;
    bool resetEncoderRaw(int j) override;
    bool resetEncodersRaw() override;
    bool setEncoderRaw(int j, double val) override;
    bool setEncodersRaw(const double *vals) override;
    bool getEncoderRaw(int j, double *v) override;
    bool getEncodersRaw(double *encs) override;
    bool getEncoderSpeedRaw(int j, double *sp) override;
    bool getEncoderSpeedsRaw(double *spds) override;
    bool getEncoderAccelerationRaw(int j, double *spds) override;
    bool getEncoderAccelerationsRaw(double *accs) override;
    ///////////////////////// END Encoder Interface

    //////////////////////// BEGIN EncoderTimed Interface
    bool getEncodersTimedRaw(double *encs, double *stamps) override;
    bool getEncoderTimedRaw(int j, double *encs, double *stamp) override;
    ///////////////////////// END EncoderTimed Interface

    //////////////////////// BEGIN IAxisInfo Interface
    bool getAxisNameRaw(int axis, std::string& name) override;
    bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
    ///////////////////////// END IAxisInfo Interface

    void run() override;
private:
    void cleanup();
    bool dealloc();

    bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
};

#endif  // YARP_DEVICE_FAKE_MOTIONCONTROLMICRO
