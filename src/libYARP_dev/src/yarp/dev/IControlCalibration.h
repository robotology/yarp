/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLCALIBRATION_H
#define YARP_DEV_ICONTROLCALIBRATION_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/CalibratorInterfaces.h> // ICalibrator

namespace yarp {
    namespace dev {
        class IControlCalibrationRaw;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlCalibrationRaw instead") IControlCalibrationRaw IControlCalibration2Raw;
#endif
        class IControlCalibration;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlCalibration instead") IControlCalibration IControlCalibration2;
#endif
        struct CalibrationParameters;
    }
}

struct YARP_dev_API yarp::dev::CalibrationParameters
{
    unsigned int type;
    double param1;
    double param2;
    double param3;
    double param4;
    double param5;
    double paramZero;
    CalibrationParameters() { type = 0; param1 = 0; param2 = 0; param3 = 0; param4 = 0; param5 = 0; paramZero = 0; }
};

/**
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibrationRaw
{
public:
    IControlCalibrationRaw(){}
    /**
     * Destructor.
     */
    virtual ~IControlCalibrationRaw() {}

    /**
     * Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool calibrationDoneRaw(int j)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrateAxisWithParamsRaw(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
    */
    virtual bool setCalibrationParametersRaw(int axis, const CalibrationParameters& params) { return false; }

};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control devices, calibration commands.
 */
class YARP_dev_API yarp::dev::IControlCalibration
{
private:
    ICalibrator *calibrator;

public:
    IControlCalibration();
    /**
     * Destructor.
     */
    virtual ~IControlCalibration() {}

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool calibrateAxisWithParams(int axis, unsigned int type, double p1, double p2, double p3)=0;

    /**
     *  Start calibration, this method is very often platform
     * specific.
     * @return true/false on success failure
     */
    virtual bool setCalibrationParameters(int axis, const CalibrationParameters& params) { return false; }

    /**
     *  Check if the calibration is terminated, on a particular joint.
     * Non blocking.
     * @return true/false
     */
    virtual bool calibrationDone(int j)=0;

    /**
     * Set the calibrator object to be used to calibrate the robot.
     * @param c pointer to the calibrator object
     * @return true/false on success failure
     */
    virtual bool setCalibrator(ICalibrator *c);

    /**
     * Calibrate robot by using an external calibrator. The external
     * calibrator must be previously set by calling the setCalibration()
     * method.
     * @return true/false on success failure
     */
    virtual bool calibrateRobot();

    virtual bool park(bool wait=true);

    /* Abort calibration, force the function calibrate() to return.*/
    virtual bool abortCalibration();

    /* Abort parking, force the function park() to return.*/
    virtual bool abortPark();
};


/* Vocabs representing the above interfaces */

constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_JOINT        = yarp::os::createVocab32('c','a','l','j');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_JOINT_PARAMS = yarp::os::createVocab32('c','l','j','p');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE              = yarp::os::createVocab32('c','a','l');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_CHANNEL      = yarp::os::createVocab32('c','a','l','c');
constexpr yarp::conf::vocab32_t VOCAB_ABORTCALIB             = yarp::os::createVocab32('a','b','c','a');
constexpr yarp::conf::vocab32_t VOCAB_ABORTPARK              = yarp::os::createVocab32('a','b','p','a');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_DONE         = yarp::os::createVocab32('c','a','l','d');
constexpr yarp::conf::vocab32_t VOCAB_PARK                   = yarp::os::createVocab32('p','a','r','k');

#endif // YARP_DEV_ICONTROLCALIBRATION_H
