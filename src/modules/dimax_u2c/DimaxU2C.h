/*
 * Copyright (C) 2006 Assif Mirza
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef KASPAR_CONTROL_DEVICE_H
#define KASPAR_CONTROL_DEVICE_H

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include "Servo.h"

namespace yarp {
    namespace dev {
        class DimaxU2C;
    }
}

/**
 * @ingroup dev_impl_motor
 *
 * A basic driver for Dimax-U2C-12 Communications board.
 * The Dimax U2C-12 card is a communications card for bridging between USB
 * and the I2C bus.
 * The driver implements the very basics of IPositionControl - that is "positionMove"
 * and "getAxes".
 * The driver is most definitely still "under development". Currently no Angle to
 * Position translation is done and certain assumptions are made  about the byte
 * layout of the motors in the I2C message.  This will be made more generic, as the
 * driver matures.
 *
 * @author Assif Mirza
 *
 */
class yarp::dev::DimaxU2C :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IPositionControlRaw,
  public yarp::dev::ImplementPositionControl<DimaxU2C, yarp::dev::IPositionControl>
{
public:
    /**
     * Constructor.
     * Creates a servo object for controlling servo motors. See Servo.h.
     */
    DimaxU2C();
    ~DimaxU2C();

    // methods to implement DeviceDriver Interface

    /**
     * Open the DimaxU2C device.
     * Initialises the position control device driver with a one to one mapping
     * of angle to encoder value with zero offset. <br>
     * The number of motors is passed in the configuration ("axes").
     * \param config is a list of parameters for this device.
     */
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // virtual methods implementing IPositionControlRaw Interface
    // POSITION CONTROL INTERFACE RAW
    virtual bool getAxes(int *ax);

    /**
     * Raw position control. Calls the servo setPosition method which
     * creates an I2C message with 3 bytes indicating the speed and
     * HI/LO byte of the encoder position. The byte groups for the motors
     * are sequential. <br>
     * For position control using motor angle, use IPositionControl::positionMove instead.
     * \param j the number of the motor (joint or axis) to control
     * \param ref the position to move the servo motor to: values in range 800-2200 are safe.
     */
    virtual bool positionMoveRaw(int j, double ref);
    virtual bool positionMoveRaw(const double *refs);
    virtual bool relativeMoveRaw(int j, double delta);
    virtual bool relativeMoveRaw(const double *deltas);
    virtual bool checkMotionDoneRaw(bool *flag);
    virtual bool checkMotionDoneRaw(int j, bool *flag);
    virtual bool setRefSpeedRaw(int j, double sp);
    virtual bool setRefSpeedsRaw(const double *spds);
    virtual bool setRefAccelerationRaw(int j, double acc);
    virtual bool setRefAccelerationsRaw(const double *accs);
    virtual bool getRefSpeedRaw(int j, double *ref);
    virtual bool getRefSpeedsRaw(double *spds);
    virtual bool getRefAccelerationRaw(int j, double *acc);
    virtual bool getRefAccelerationsRaw(double *accs);
    virtual bool stopRaw(int j);
    virtual bool stopRaw();
    //
    // END Position Control Raw INTERFACE
private:
    Servo *servos;
    int numJoints;
    double *speeds;
    double *accels;

};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_dimax_u2c dimax_u2c
 *
 *  A motor driver, see yarp::dev::DimaxU2C.
 *
 */


#endif
