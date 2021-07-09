/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICURRENTCONTROL_H
#define YARP_DEV_ICURRENTCONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ICurrentControlRaw;
        class ICurrentControl;
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing current control.
 */
class YARP_dev_API yarp::dev::ICurrentControl
{
public:
    /**
     * Destructor.
     */
    virtual ~ICurrentControl() {}

    /**
     * Retrieves the number of controlled axes from the current physical interface.
     * @param ax returns the number of controlled axes.
     * @return true/false on success/failure
     */
    virtual bool getNumberOfMotors(int *ax) = 0;

    /** Get the instantaneous current measurement for a single motor
    * @param m motor number
    * @param curr pointer to the result value. Value is expressed in amperes.
    * @return true/false on success/failure
    */
    virtual bool getCurrent(int m, double *curr) = 0;

    /** Get the instantaneous current measurement for all motors
    * @param currs pointer to the array that will store the output. Values are expressed in amperes.
    * @return true/false on success/failure
    */
    virtual bool getCurrents(double *currs) = 0;

    /** Get the full scale of the current measurement for a given motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param m motor number
    * @param min minimum current of the motor m
    * @param max maximum current of the motor m
    * @return true/false on success/failure
    */
    virtual bool getCurrentRange(int m, double *min, double *max) = 0;

    /** Get the full scale of the current measurements for all motors motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param min pointer to the array that will store minimum currents
    * @param max pointer to the array that will store maximum currents
    * @return true/false on success/failure
    */
    virtual bool getCurrentRanges(double *min, double *max) = 0;

    /** Set the reference value of the currents for all motors.
    * @param currs the array containing the reference current values. Values are expressed in amperes.
    * @return true/false on success/failure
    */
    virtual bool setRefCurrents(const double *currs) = 0;

    /** Set the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m. Value is expressed in amperes.
    * @return true/false on success/failure
    */
    virtual bool setRefCurrent(int m, double curr) = 0;

    /**  Set the reference value of the current for a group of motors.
    * @param n_motor size of motors ans currs arrays
    * @param motors  pointer to the array containing the list of motor numbers
    * @param currs   pointer to the array specifying the new current references
    * @return true/false on success/failure
    */
    virtual bool setRefCurrents(const int n_motor, const int *motors, const double *currs) = 0;

   /** Get the reference value of the currents for all motors.
     * @param currs pointer to the array to be filled with reference current values. Values are expressed in amperes.
     * @return true/false on success/failure
     */
    virtual bool getRefCurrents(double *currs) = 0;

    /** Get the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m. Value is expressed in amperes.
    * @return true/false on success/failure
    */
    virtual bool getRefCurrent(int m, double *curr) = 0;
};

/**
 *
 * Interface for control boards implementing current control.
 */
class YARP_dev_API yarp::dev::ICurrentControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~ICurrentControlRaw() {}

    /**
    * Retrieves the number of controlled motors from the current physical interface.
    * @param number returns the number of controlled motors.
    * @return true/false on success/failure
    */
    virtual bool getNumberOfMotorsRaw(int *number) = 0;

    /** Get the instantaneous current measurement for a single motor.
    * @param m motor number
    * @param curr pointer to the result value
    * @return true/false on success/failure
    */
    virtual bool getCurrentRaw(int m, double *curr)=0;

    /** Get the instantaneous current measurement for all motors.
    * @param currs pointer to the array that will store the output
    * @return true/false on success/failure
    */
    virtual bool getCurrentsRaw(double *currs)=0;

    /** Get the full scale of the current measurement for a given motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param m motor number
    * @param min minimum current of the motor m
    * @param max maximum current of the motor m
    * @return true/false on success/failure
    */
    virtual bool getCurrentRangeRaw(int m, double *min, double *max)=0;

    /** Get the full scale of the current measurements for all motors motor (e.g. -20A +20A)
    * Reference values set by user with methods such as setRefCurrent() should be in this range.
    * This method is not related to the current overload protection methods belonging to the iAmplifierControl interface.
    * @param min pointer to the array that will store minimum currents
    * @param max pointer to the array that will store maximum currents
    * @return true/false on success/failure
    */
    virtual bool getCurrentRangesRaw(double *min, double *max)=0;

    /** Set the reference value of the currents for all motors.
    * @param currs the array containt the reference current values
    * @return true/false on success/failure
    */
    virtual bool setRefCurrentsRaw(const double *currs)=0;

    /** Set the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m
    * @return true/false on success/failure
    */
    virtual bool setRefCurrentRaw(int m, double curr)=0;

    /**  Set the reference value of the current for a group of motors.
     * @param n_motor size of motors ans currs arrays
     * @param motors pointer to the array of motor numbers
     * @param refs   pointer to the array specifying the new current reference
     * @return true/false on success/failure
     */
    virtual bool setRefCurrentsRaw(const int n_motor, const int *motors, const double *currs) = 0;

    /** Get the reference value of the currents for all motors.
    * @param currs pointer to the array to be filled with reference current values.
    * @return true/false on success/failure
    */
    virtual bool getRefCurrentsRaw(double *currs)=0;

    /** Get the reference value of the current for a single motor.
    * @param m motor number
    * @param curr the current reference value for motor m
    * @return true/false on success/failure
    */
    virtual bool getRefCurrentRaw(int m, double *curr)=0;
};

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_CURRENTCONTROL_INTERFACE = yarp::os::createVocab32('i','c','u','r');
// methods names

constexpr yarp::conf::vocab32_t VOCAB_CURRENT_REF          = yarp::os::createVocab32('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_CURRENT_REFS         = yarp::os::createVocab32('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_CURRENT_REF_GROUP    = yarp::os::createVocab32('r','e','f','g');
constexpr yarp::conf::vocab32_t VOCAB_CURRENT_RANGE        = yarp::os::createVocab32('r','n','g');
constexpr yarp::conf::vocab32_t VOCAB_CURRENT_RANGES       = yarp::os::createVocab32('r','n','g','s');

#endif // YARP_DEV_ICURRENTCONTROL_H
