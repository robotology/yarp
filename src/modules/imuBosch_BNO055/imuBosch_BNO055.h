// Copyright: (C) 2016 iCub Facility
// Authors: Alberto Cardellino <alberto.cardellino@iit.it>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef BOSCH_IMU_DEVICE
#define BOSCH_IMU_DEVICE

#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/SerialInterfaces.h>
#include <yarp/dev/GenericSensorInterfaces.h>


namespace yarp {
    namespace dev {
        class BoschIMU;
    }
}

/* Serial protocol desciption
 *
 *  Write operation on a register:
 * | Byte 1 |  Byte 2 |  Byte 3  | Byte 4 |  Byte 5 |  ...  |  Byte (n+4) |
 * | Start  |  Write  |  Reg Addr| Length |  Data 1 |  ...  |   Data n    |
 * |  0xAA  |   0x00  |  <...>   | <...>  |  <...>  |  ...  |    <...>    |
 *
 * Response to write command:
 * | 0xEE   | <code>  |
 * 0x01: Write_success
 * all other values are errors ... TODO: improve doc
 *
 *  Read operation on a register:
 * | Byte 1 |  Byte 2 |  Byte 3  | Byte 4 |
 * | Start  |  Read   |  Reg Addr| Length |
 * |  0xAA  |  0x01   |  <...>   | <...>  |
 *
 * Response to a succesfull read command:
 * | Byte 1 |  Byte 2 |  Byte 3 |  ...  |  Byte (n+2) |
 * |  Resp  |  Length |  Data 1 |  ...  |   Data n    |
 * |  0xBB  |  <...>  |  <...>  |  ...  |    <...>    |
 *
 * Response to a failed read command:
 * | Byte 1 |  Byte 2 |
 * |  Resp  |  Status |
 * |  0xEE  |  <...>  |
 *
 * Read error code:  TODO
 */

#define MAX_MSG_LENGTH  128

// Commands
#define START_BYTE  0xAA
#define WRITE_CMD   0x00
#define READ_CMD    0x01

// Responses
#define REPLY_HEAD  0xBB
#define ERROR_HEAD  0xEE
#define WRITE_SUCC  0x01
#define READ_FAIL   0x02
#define WRITE_FAIL  0x03

// Error code
#define REGISTER_NOT_READY  0x07

// Registers
// Page 0       // Device has 2 pages of registers
#define REG_CHIP_ID         0x00
#define REG_SW_VERSION      0x04    // 2 software revision bytes
#define REG_BOOTLOADER      0x06    // 1 byte  bootloader version
#define REG_PAGE_ID         0x07    // page ID number

#define REG_ACC_DATA        0x08    // 3*2 bytes: LSB first (LSB 0x08, MSB 0x09) for X
#define REG_MAGN_DATA       0x0E    // 3*2 bytes: LSB first
#define REG_GYRO_DATA       0x14    // 3*2 bytes: LSB first
#define REG_RPY_DATA        0x1A    // 3*2 bytes: LSB first  (raw order is Yaw, Roll, Pitch)
#define REG_QUATERN_DATA    0x20    // 4*2 bytes: LSB first  (raw order is w, x, y, z)
#define REG_GRAVITY         0x2E    // Gravity Vector data
#define REG_CALIB_STATUS    0x35    // Check if sensors are calibrated, 2 bits each. SYS - GYRO - ACC - MAG. 3 means calibrated, 0 not calbrated
#define REG_SYS_CLK_STATUS  0x38    // only 1 last LSB
#define REG_SYS_STATUS      0x39
#define REG_SYS_ERR         0x3A
#define REG_UNIT_SEL        0x3B
#define REG_OP_MODE         0x3D
#define REG_POWER_MODE      0x3E
#define REG_SYS_TRIGGER     0x3F

// Values
#define CONFIG_MODE         0x00
#define AMG_MODE            0x07
#define IMU_MODE            0x08
#define M4G_MODE            0x0A
#define NDOF_MODE           0x0C

// Sys trigger values (in OR if more than one is to be activated)
#define TRIG_EXT_CLK_SEL    0x80    // 1 for external clock (if available), 0 for internal clock
#define TRIG_RESET_INT      0x40    // reset interrupts
#define TRIG_RESET_SYSTEM   0x20    // reset system
#define TRIG_SELF_TEST      0x01    // Start self test

// Time to wait while switching to and from config_mode & any operation_mode
#define SWITCHING_TIME   0.020  //   20ms

/**
*  @ingroup dev_impl_wrapper
*
* \section BoschIMU Description of input parameters
*
*  This device will connect to the proper analogServer and read the data broadcasted making them available to use for the user application. It also made available some function to check and control the state of the remote sensor.
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units | Default Value | Required  | Description   | Notes |
* |:--------------:|:------:|:-----:|:-------------:|:--------: |:-------------:|:-----:|
* | comport        | string |       |               | Yes       | full name of device file  | ex '/dev/ttyUSB0' |
* | baudrate       | int    | Hz    |               | Yes       | baudrate setting of COM port | ex 115200 |
*
**/

class yarp::dev::BoschIMU:   public yarp::dev::DeviceDriver,
                             public yarp::os::RateThread,
                             public yarp::dev::IGenericSensor
{
protected:

    yarp::os::Semaphore         mutex;
    bool                        verbose;
    short                       status;
    int                         nChannels;
    yarp::sig::Vector           data;
    yarp::sig::Vector           quaternion;
    yarp::sig::Vector           RPY_angle;
    double                      timeStamp;

    bool                        checkError;

    int                         fd_ser;
    yarp::os::ResourceFinder    rf;

    unsigned char command[MAX_MSG_LENGTH];
    unsigned char response[MAX_MSG_LENGTH];

    bool checkWriteResponse(unsigned char *response);
    bool checkReadResponse(unsigned char *response);

    void printBuffer(unsigned char *buffer, int length);
    int  readBytes(unsigned char *buffer, int bytes);
    void dropGarbage();

    long int           totMessagesRead;
    yarp::sig::Vector  errorCounter;

    void readSysError();

    bool sendReadCommand(unsigned char register_add, int len, unsigned char* buf, std::string comment = "");
    bool sendWriteCommand(unsigned char register_add, int len, unsigned char* cmd, std::string comment = "");

public:
    BoschIMU();

    ~BoschIMU();

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    /*
     * Read a vector from the sensor.
     * @param out a vector containing the sensor's last readings.
     * @return true/false success/failure
     */
    virtual bool read(yarp::sig::Vector &out);

    /**
     * Get the number of channels of the sensor.
     * @param nc pointer to storage, return value
     * @return true/false success/failure
     */
    virtual bool getChannels(int *nc);

    /**
     * Calibrate the sensor, single channel.
     * @param ch channel number
     * @param v reset valure
     * @return true/false success/failure
     */
    virtual bool calibrate(int ch, double v);

    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();
};


#endif  // BOSCH_IMU_DEVICE
