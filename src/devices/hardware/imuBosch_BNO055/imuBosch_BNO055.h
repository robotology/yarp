/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BOSCH_IMU_DEVICE
#define BOSCH_IMU_DEVICE

#include <atomic>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/math/Quaternion.h>
#include <mutex>


/* Serial protocol description
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
 * Response to a successful read command:
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

constexpr int MAX_MSG_LENGTH = 128;

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

#define BNO055_ID           0xA0

#define RESP_HEADER_SIZE                 2
// Time to wait while switching to and from config_mode & any operation_mode
#define SWITCHING_TIME                   0.020  //   20ms
#define TIME_REPORT_INTERVAL             30
//number of attempts of sending config command
#define ATTEMPTS_NUM_OF_SEND_CONFIG_CMD  3



/**
*  @ingroup dev_impl_analog_sensors
*
* \brief `imuBosch_BNO055`: This device will connect to the proper analogServer
* and read the data broadcasted making them available to use for the user
* application. It also made available some function to check and control the
* state of the remote sensor.
*
* \section imuBosch_BNO055_device_parameters Description of input parameters
*
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units | Default Value | Required  | Description   | Notes |
* |:--------------:|:------:|:-----:|:-------------:|:--------: |:-------------:|:-----:|
* | comport        | string |       |               | Yes if i2c not specified | full name of device file  | ex '/dev/ttyUSB0', it is mutually exclusive with 'i2c' parameter|
* | baudrate       | int    | Hz    |               | Yes if i2c not specified | baudrate setting of COM port | ex 115200, used only with serial configuration |
* | i2c            | string |       |               | Yes if comport not specified | full name of device file  | ex '/dev/i2c-5', it is mutually exclusive with 'comport' parameter, necessary for i2c configuration|
* | period         | int    | ms    |       10      | No       | period of the thread | |
* | sensor_name    | string |       | sensor_imu_bosch_bno055 | No | full name of the device | |
* | frame_name     | string |       | set same as `sensor_name` | No | full name of the sensor frame in which the measurements are expressed | |
**/

class BoschIMU:
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IGenericSensor,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IOrientationSensors
{
protected:
    bool                        verbose;              ///< Flag to get verbose output
    short                       status;               ///< device status - UNUSED
    int                         nChannels;            ///< number of channels in the output port. Default 12. If 16, also includes quaternion data
    yarp::sig::Vector           data;                 ///< sensor data buffer
    yarp::sig::Vector           data_tmp;             ///< sensor data temporary buffer
    yarp::math::Quaternion      quaternion;           ///< orientation in quaternion representation
    yarp::math::Quaternion      quaternion_tmp;       ///< orientation in quaternion representation
    yarp::sig::Vector           RPY_angle;            ///< orientation in Euler angle representation
    double                      m_timeStamp;          ///< device timestamp
    double                      timeLastReport;       ///< timestamp of last reported data
    mutable std::mutex             mutex;        ///< mutex to avoid resource clash
    bool                        i2c_flag;             ///< flag to check if device connected through i2c commununication

    bool                        checkError;           ///< flag to check read error of sensor data

    int                         fd;                   ///< file descriptor to open device at system level
    size_t                      responseOffset;
    yarp::os::ResourceFinder    rf;                   ///< resource finder object to load config parameters

    using ReadFuncPtr = bool (BoschIMU::*)(unsigned char, int, unsigned char*, std::string);  ///< Functor to choose between i2c or serial comm
    ReadFuncPtr readFunc;                             ///< Functor object

    unsigned char command[MAX_MSG_LENGTH];            ///< packet to be written to the device
    unsigned char response[MAX_MSG_LENGTH];           ///< packet to be read from the device


    bool checkWriteResponse(unsigned char *response);
    bool checkReadResponse(unsigned char *response);

    void printBuffer(unsigned char *buffer, int length);
    int  readBytes(unsigned char *buffer, int bytes);
    void dropGarbage();

    long int           totMessagesRead;
    yarp::sig::Vector  errorCounter;
    yarp::sig::Vector  errorReading;

    void readSysError();
    // Serial
    bool sendReadCommandSer(unsigned char register_add, int len, unsigned char* buf, std::string comment = "");
    bool sendWriteCommandSer(unsigned char register_add, int len, unsigned char* cmd, std::string comment = "");
    bool sendAndVerifyCommandSer(unsigned char register_add, int len, unsigned char* cmd, std::string comment);

    // i2c
    bool sendReadCommandI2c(unsigned char register_add, int len, unsigned char* buf, std::string comment = "");

    int errs;
    std::atomic<bool> dataIsValid;

public:
    BoschIMU();

    ~BoschIMU();

    /**
     * Open the device and set up parameters/communication
     * @param[in] config searchable object with desired configuration parameters
     * @return true/false success/failure
     */
    bool open(yarp::os::Searchable& config) override;

    /**
     * Close the device
     * @return true/false success/failure
     */
    bool close() override;

    /**
     * Read a vector from the sensor.
     * @param[out] out a vector containing the sensor's last readings.
     * @return true/false success/failure
     */
    bool read(yarp::sig::Vector &out) override;

    /**
     * Get the number of channels of the sensor.
     * @param[out] nc pointer to storage, return value
     * @return true/false success/failure
     */
    bool getChannels(int *nc) override;

    /**
     * Calibrate the sensor, single channel.
     * @param[in] ch channel number
     * @param[in] v reset value
     * @return true/false success/failure
     */
    bool calibrate(int ch, double v) override;

    /* IThreeAxisGyroscopes methods */
    /**
     * Get the  number of three axis gyroscopes in the device
     * @return 1
     */
    size_t getNrOfThreeAxisGyroscopes() const override;

    /**
     * Get the status of three axis gyroscope
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @return MAS_OK/MAS_ERROR if status ok/failure
     */
    yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;

    /**
     * Get the name of three axis gyroscope
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] name name of the sensor
     * @return true/false success/failure
     */
    bool getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const override;

    /**
     * Get the name of the frame in which three axis gyroscope measurements are expressed
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] frameName name of the sensor frame
     * @return true/false success/failure
     */
    bool getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const override;

    /**
     * Get three axis gyroscope measurements
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] out 3D angular velocity measurement in deg/s
     * @param[out] timestamp timestamp of measurement
     * @return true/false success/failure
     */
    bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisLinearAccelerometers methods */
    /**
     * Get the  number of three axis linear accelerometers in the device
     * @return 1
     */
    size_t getNrOfThreeAxisLinearAccelerometers() const override;

    /**
     * Get the status of three axis linear accelerometer
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @return MAS_OK/MAS_ERROR if status ok/failure
     */
    yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;

    /**
     * Get the name of three axis linear accelerometer
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] name name of the sensor
     * @return true/false success/failure
     */
    bool getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const override;

    /**
     * Get the name of the frame in which three axis linear accelerometer measurements are expressed
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] frameName name of the sensor frame
     * @return true/false success/failure
     */
    bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const override;

    /**
     * Get three axis linear accelerometer measurements
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] out 3D linear acceleration measurement in m/s^2
     * @param[out] timestamp timestamp of measurement
     * @return true/false success/failure
     */
    bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisMagnetometers methods */
    /**
     * Get the  number of three axis magnetometers in the device
     * @return 1
     */
    size_t getNrOfThreeAxisMagnetometers() const override;

    /**
     * Get the status of three axis magnetometer
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @return MAS_OK/MAS_ERROR if status ok/failure
     */
    yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;

    /**
     * Get the name of three axis magnetometer
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] name name of the sensor
     * @return true/false success/failure
     */
    bool getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const override;

    /**
     * Get the name of the frame in which three axis magnetometer measurements are expressed
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] frameName name of the sensor frame
     * @return true/false success/failure
     */
    bool getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const override;

    /**
     * Get three axis magnetometer measurements
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] out 3D magnetometer measurement
     * @param[out] timestamp timestamp of measurement
     * @return true/false success/failure
     */
    bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IOrientationSensors methods */
    /**
     * Get the  number of orientation sensors in the device
     * @return 1
     */
    size_t getNrOfOrientationSensors() const override;

    /**
     * Get the status of orientation sensor
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @return MAS_OK/MAS_ERROR if status ok/failure
     */
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;

    /**
     * Get the name of orientation sensor
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] name name of the sensor
     * @return true/false success/failure
     */
    bool getOrientationSensorName(size_t sens_index, std::string &name) const override;

    /**
     * Get the name of the frame in which orientation sensor measurements are expressed
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] frameName name of the sensor frame
     * @return true/false success/failure
     */
    bool getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;

    /**
     * Get orientation sensor measurements
     * @param[in] sens_index sensor index (must be 0 in the case BoschIMU)
     * @param[out] out RPY Euler angles in deg
     * @param[out] timestamp timestamp of measurement
     * @return true/false success/failure
     */
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

    /**
     * Initialize process with desired device configurations
     * @return true/false success/failure
     */
    bool threadInit() override;


    /**
     * Terminate communication with the device and release the thread.
     */
    void threadRelease() override;

    /**
     * Update loop where measurements are read from the device.
     */
    void run() override;

private:
    yarp::dev::MAS_status genericGetStatus(size_t sens_index) const;
    bool genericGetSensorName(size_t sens_index, std::string &name) const;
    bool genericGetFrameName(size_t sens_index, std::string &frameName) const;

    std::string m_sensorName;     ///< name of the device
    std::string m_frameName;      ///< name of the frame in which the measurements will be expressed
};


#endif  // BOSCH_IMU_DEVICE
