/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Ze Ji
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*
 * YARP Driver for Dynamixel AX-12, controlled using USB to Dynamixel Adapter
 * The default IDs for the motors are for Kaspar robot. As different robots would have different settings,
 * it is necessary to define the IDs, this can be done via a configuration file. In this example, check the
 * file sensorindex.cfg. The format is:
 * SENSORINDEX 16 101 116 132 102 117 133 103 118 134 107 119 135 106 104 105 108
 * This needs to be done by running configure(), before opening the device
 *
 * As it is using libftdi library, it is not possible to identify a device by specifying the serial port number, such as
 * /dev/ttyUSB0
 * Instead, this driver requires precise information of the ftdi device, such as the dynamixel usb manufacture id, serial number, etc. See class FtdiDeviceSettings for details.
 *
 * The motor does not support Torque control, but provide torque feedback. Therefore, several functions
 * have been implemented for such purpose, though not very rigorous.
 *
 * Author: Ze Ji - z.ji@herts.ac.uk (University of Hertfordshire, UK), 2010
 * as part of the RoboSkin project (EU FP7).
 * Part of the work is initially from Sven Magg, 2009
 */

#ifndef DynamixelAX12Driverh
#define DynamixelAX12Driverh

//#define __DEBUG__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ftdi.h>
//#include <libftdi/ftdi.h>
#include <usb.h>
#include <iostream>
#include <yarp/os/Time.h>

#include <mutex>

#define MOTION_COMPLETION_TOLERANCE 3

/** defines for AX12 instruction codes ********************************/

#define INST_PING         0x01
#define INST_READ         0x02
#define INST_WRITE        0x03
#define INST_REGWRITE     0x04
#define INST_ACTION       0x05
#define INST_RESET        0x06
#define INST_SYNCWRITE    0x83

/** defines for AX12 general ids **************************************/
#define AX12_BROADCAST_ID 0xFE

/** defines for device parameters - EEPROM *****************************
    100 added if parameter has length 2 */

#define CT_MODEL_NUMBER         00
#define CT_FIRMWARE_VERSION      2
#define CT_RETURN_DELAY_TIME     5
#define CT_CW_ANGLE_LIMIT       06
#define CT_CWW_ANGLE_LIMIT      08
#define CT_LIMIT_TEMPERATURE_H  11
#define CT_LIMIT_VOLTAGE_L      12
#define CT_LIMIT_VOLTAGE_H      13
#define CT_MAX_TORQUE           14
#define CT_STATUS_RETURN_LEVEL  16

/** defines for device parameters - RAM ********************************/

#define CT_TORQUE_ENABLE        24
#define CT_CW_COMP_MARGIN       26
#define CT_CWW_COMP_MARGIN      27
#define CT_CW_COMP_SLOPE        28
#define CT_CWW_COMP_SLOPE       29
#define CT_GOAL_POSITION        30
#define CT_MOVING_SPEED         32
#define CT_TORQUE_LIMIT         34
#define CT_PRESENT_POSITION     0x24
#define CT_PRESENT_SPEED        0x26
#define CT_PRESENT_LOAD         0x28
#define CT_PRESENT_VOLTAGE      42
#define CT_PRESENT_TEMPERATURE  43
#define CT_REG_INSTRUCTION      44
#define CT_MOVING               46
#define CT_LOCK                 47
#define CT_PUNCH                48

enum ErrorCode {
    VOLTAGE_ERROR,
    ANGLE_ERROR,
    OVERHEATING_ERROR,
    RANGE_ERROR,
    CHECKSUM_ERROR,
    OVERLOAD_ERROR,
    INSTRUCTION_ERROR,
    OK
};

using namespace yarp::os;
using namespace yarp::dev;

/**
 * FtdiDeviceSettings contains information to identify specific device
 * Such a device can contain information:
 * such as: Manufacturer: FTDI, Description: FT232R USB UART, Serial A7003MhG
 */
class FtdiDeviceSettings
{
public:
    int vendor; //0x0403 normally. Can be found by lsusb on linux
    int product; //0x6001   Can be found by lsusb on linux
    char description[100]; // Contains the description
    char manufacturer[100]; // Contains manufacturer
    char serial[100]; // used mainly, as this seems unique
    int baudrate; // if ((retCode = ftdi_set_baudrate(&ftdic, config2.SerialParams.baudrate)) != 0)
    int readtimeout;
    // int bits;                    //    if (ftdi_set_line_property(&ftdic, BITS_8, STOP_BIT_1, NONE) == -1)
    // int parity;
    // int sbit;
    int flowctrol; // flow control to use. should be SIO_DISABLE_FLOW_CTRL, SIO_RTS_CTS_HS, SIO_DTR_DSR_HS or SIO_XON_XOFF_HS

    unsigned int write_chunksize; // e.g. ftdi_write_data_set_chunksize(&ftdic, 3);
    unsigned int read_chunksize; // ftdi_read_data_set_chunksize(&ftdic, 256);
};

/**
* @ingroup dev_impl_motor
*
* \brief `dynamixelAX12Ftdi`: Documentation to be added
*/
class DynamixelAX12FtdiDriver :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPositionControl,
        public yarp::dev::ITorqueControl,
        public yarp::dev::IEncoders
{
private:
    /** Handle to FTDI device */
    struct ftdi_context ftdic;
    /** Switch set if device is open and ready */
    bool deviceOpen;

    ErrorCode checkAnswerPacket(unsigned char* packet, const char*& message);

    std::mutex mutex;

    unsigned char *jointNumbers;

    int numOfAxes; // default = 16

public:

    /** Constructor
        Initializes handle but doesn't open channel yet
     */
    DynamixelAX12FtdiDriver();

    /** Destructor
        Closes connection and destroys device object
     */
    virtual ~DynamixelAX12FtdiDriver();

    /** Open device
        Opens and configures the device.

        @param config Config file containing string pairs for parameters
        @return true on success
     */
    bool open(yarp::os::Searchable& config) override;

    /** Close device
        Closes the device and shuts down connection.

        @return true on success
     */
    bool close() override;

    /** Configure device online
        Configures parts of the device that can be configures online.

        @param config Config file containing string pairs for parameters
        @return true on success
     */
    bool configure(yarp::os::Searchable& config);

    /** Send instruction to device
        Send an instruction to a device of given ID. The instruction has to be a byte arry containing
        the AX12 instruction code beginning with the instruction, the address and the parameters.
        Header and checksum are written automatically.

        @param id The hex id of the device to be contacted
        @param inst Byte array containing the instruction body (instruction, address, parameters)
        @return The content of the return packet of the device
     */
    virtual int sendCommand(unsigned char id, unsigned char inst[], int size, unsigned char ret[], int &retSize);

    /** Read parameter from motor
        Requests the value of a parameter from motor.

        @param id The id of the device to be contacted
        @param param encodes address in control table and size of parameter (2 Bytes => address + 100, 1 byte => address)
        @return value if read successful or -1
     */
    virtual int readParameter(unsigned char id, unsigned char param);

    /**
     * Interfaces methods
     */
    yarp::dev::ReturnValue getAxes(int *ax) override;

    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const double *refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(bool *flag) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double *spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double *ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(double *spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(double *accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop() override;

    yarp::dev::ReturnValue getRefTorques(double *t) override;
    yarp::dev::ReturnValue getRefTorque(int j, double *t) override;
    yarp::dev::ReturnValue setTorques(const double *t);
    yarp::dev::ReturnValue setTorque(int j, double t);
    yarp::dev::ReturnValue setTorquePid(int j, const Pid &pid);
    yarp::dev::ReturnValue getTorque(int j, double *t) override;
    yarp::dev::ReturnValue getTorques(double *t) override;
    yarp::dev::ReturnValue setTorquePids(const Pid *pids);
    yarp::dev::ReturnValue setTorqueErrorLimit(int j, double limit);
    yarp::dev::ReturnValue setTorqueErrorLimits(const double *limits);
    yarp::dev::ReturnValue getTorqueError(int j, double *err);
    yarp::dev::ReturnValue getTorqueErrors(double *errs);
    yarp::dev::ReturnValue getTorquePidOutput(int j, double *out);
    yarp::dev::ReturnValue getTorquePidOutputs(double *outs);
    yarp::dev::ReturnValue getTorquePid(int j, Pid *pid);
    yarp::dev::ReturnValue getTorquePids(Pid *pids);
    yarp::dev::ReturnValue getTorqueErrorLimit(int j, double *limit);
    yarp::dev::ReturnValue getTorqueErrorLimits(double *limits);
    yarp::dev::ReturnValue resetTorquePid(int j);
    yarp::dev::ReturnValue disableTorquePid(int j);
    yarp::dev::ReturnValue enableTorquePid(int j);
    yarp::dev::ReturnValue setTorqueOffset(int j, double v);

    yarp::dev::ReturnValue getBemfParam(int j, double *bemf);
    yarp::dev::ReturnValue setBemfParam(int j, double bemf);

    yarp::dev::ReturnValue resetEncoder(int j) override;
    yarp::dev::ReturnValue resetEncoders() override;

    yarp::dev::ReturnValue setEncoder(int j, double val) override;
    yarp::dev::ReturnValue setEncoders(const double *vals) override;
    /**
     * Get the angle of servo.
     */
    yarp::dev::ReturnValue getEncoder(int j, double *v) override;
    yarp::dev::ReturnValue getEncoders(double *encs) override;
    yarp::dev::ReturnValue getEncoderSpeed(int j, double *sp) override;
    yarp::dev::ReturnValue getEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getEncoderAcceleration(int j, double *spds) override;
    yarp::dev::ReturnValue getEncoderAccelerations(double *accs) override;

    using yarp::dev::ITorqueControl::setRefTorques;
    yarp::dev::ReturnValue setRefTorques(const double* t) override;

    yarp::dev::ReturnValue setRefTorque(int j, double t) override;
    yarp::dev::ReturnValue getTorqueRange(int j, double* min, double* max) override;
    yarp::dev::ReturnValue getTorqueRanges(double* min, double* max) override;

    // multiple joint version
    yarp::dev::ReturnValue positionMove(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDone(const int n_joint, const int *joints, bool *flag) override;
    yarp::dev::ReturnValue setTrajSpeeds(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeeds(const int n_joint, const int *joints, double *spds) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int *joints, double *accs) override;
    yarp::dev::ReturnValue stop(const int n_joint, const int *joints) override;

    yarp::dev::ReturnValue getTargetPosition(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetPositions(double *refs) override;
    yarp::dev::ReturnValue getTargetPositions(const int n_joint, const int *joints, double *refs) override;
    yarp::dev::ReturnValue setRefTorques(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params) override;
    yarp::dev::ReturnValue setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params) override;

private:

    double *positions;
    double *speeds;

    struct ftdi_device_list *devlist;
    struct ftdi_device_list *curdev;
    /// torques is int type, because there is no true unit that the normalised values can be converted to ... [0 1023]
    int *torques;

    int normalisePosition(double position);
    int normaliseSpeed(double speed);

    /**
     * Initialise motor index.
     * @param sensorIndex a bottle containing a list of values of sensor indexes
     * for kaspar, can use sensorindex.cfg as default file
     */
    bool initMotorIndex(yarp::os::Bottle *sensorIndex);

    int syncSendCommand(unsigned char id, unsigned char inst[], int size, unsigned char ret[], int &retSize);

    //bool lock;    // lock to ensure sendcommand can be only accessed by one request at one time.
};

//#undef __DEBUG__

#endif
