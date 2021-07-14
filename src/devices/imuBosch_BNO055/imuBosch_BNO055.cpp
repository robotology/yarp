/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <arpa/inet.h>
#include <cerrno>   // Error number definitions
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>   // File control definitions
#include <iostream>
#include <termios.h> // terminal io (serial port) interface
#include <unistd.h>

#include <linux/i2c-dev.h>
#ifdef I2C_HAS_SMBUS_H
extern "C" {
# include <i2c/smbus.h>
}
#endif
#include <linux/kernel.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <mutex>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>
#include <yarp/os/Time.h>

#include "imuBosch_BNO055.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(IMUBOSCH_BNO055, "yarp.device.imuBosch_BNO055")
constexpr uint8_t i2cAddrA = 0x28;
}

//constexpr uint8_t i2cAddrB = 0x29;

BoschIMU::BoschIMU() : PeriodicThread(0.02),
    verbose(false),
    status(0),
    nChannels(12),
    m_timeStamp(0.0),
    timeLastReport(0.0),
    i2c_flag(false),
    checkError(false),
    fd(0),
    responseOffset(0),
    readFunc(&BoschIMU::sendReadCommandSer),
    totMessagesRead(0),
    errs(0),
    dataIsValid(false)
{
    data.resize(12);
    data.zero();
    data_tmp.resize(12);
    data_tmp.zero();
    errorCounter.resize(11);
    errorCounter.zero();
}

BoschIMU::~BoschIMU() = default;


bool BoschIMU::open(yarp::os::Searchable& config)
{
    //debug
    yCTrace(IMUBOSCH_BNO055, "Parameters are:\n\t%s", config.toString().c_str());

    if(!config.check("comport") && !config.check("i2c"))
    {
        yCError(IMUBOSCH_BNO055) << "Params 'comport' and 'i2c' not found";
        return false;
    }

    if (config.check("comport") && config.check("i2c"))
    {
        yCError(IMUBOSCH_BNO055) << "Params 'comport' and 'i2c' both specified";
        return false;
    }

    i2c_flag = config.check("i2c");

    readFunc = i2c_flag ? &BoschIMU::sendReadCommandI2c : &BoschIMU::sendReadCommandSer;

    // In case of reading through serial the first two bytes of the response are the ack, so
    // they need to be discarded when publishing the data.
    responseOffset = i2c_flag ? 0 : 2;

    if (i2c_flag)
    {
        if (!config.find("i2c").isString())
        {
            yCError(IMUBOSCH_BNO055) << "i2c param malformed, it should be a string, aborting.";
            return false;
        }

        std::string i2cDevFile = config.find("i2c").asString();
        fd = ::open(i2cDevFile.c_str(), O_RDWR);

        if (fd < 0)
        {
            yCError(IMUBOSCH_BNO055, "Can't open %s, %s", i2cDevFile.c_str(), strerror(errno));
            return false;
        }

        if (::ioctl(fd, I2C_SLAVE, i2cAddrA) < 0)
        {
            yCError(IMUBOSCH_BNO055, "ioctl failed on %s, %s", i2cDevFile.c_str(), strerror(errno));
            return false;
        }

    }
    else
    {
        fd = ::open(config.find("comport").toString().c_str(), O_RDWR | O_NOCTTY );
        if (fd < 0) {
            yCError(IMUBOSCH_BNO055, "Can't open %s, %s", config.find("comport").toString().c_str(), strerror(errno));
            return false;
        }
        //Get the current options for the port...
        struct termios options;
        tcgetattr(fd, &options);

        cfmakeraw(&options);

        //set the baud rate to 115200
        int baudRate = B115200;
        cfsetospeed(&options, baudRate);
        cfsetispeed(&options, baudRate);

        //set the number of data bits.
        options.c_cflag &= ~CSIZE;  // Mask the character size bits
        options.c_cflag |= CS8;

        //set the number of stop bits to 1
        options.c_cflag &= ~CSTOPB;

        //Set parity to None
        options.c_cflag &=~PARENB;

        //set for non-canonical (raw processing, no echo, etc.)
        //     options.c_iflag = IGNPAR; // ignore parity check
        options.c_oflag = 0; // raw output
        options.c_lflag = 0; // raw input

        // SET NOT BLOCKING READ
        options.c_cc[VMIN]  = 0;   // block reading until RX x characters. If x = 0, it is non-blocking.
        options.c_cc[VTIME] = 2;   // Inter-Character Timer -- i.e. timeout= x*.1 s

        //Set local mode and enable the receiver
        options.c_cflag |= (CLOCAL | CREAD);

        tcflush(fd, TCIOFLUSH);

        //Set the new options for the port...
        if ( tcsetattr(fd, TCSANOW, &options) != 0)
        {
            yCError(IMUBOSCH_BNO055, "Configuring comport failed");
            return false;
        }

    }

    nChannels = config.check("channels", Value(12)).asInt32();

    double period = config.check("period",Value(10),"Thread period in ms").asInt32() / 1000.0;
    setPeriod(period);

    if (config.check("sensor_name") && config.find("sensor_name").isString())
    {
        m_sensorName = config.find("sensor_name").asString();
    }
    else
    {
        m_sensorName = "sensor_imu_bosch_bno055";
        yCWarning(IMUBOSCH_BNO055) << "Parameter \"sensor_name\" not set. Using default value  \"" << m_sensorName << "\" for this parameter.";
    }

    if (config.check("frame_name") && config.find("frame_name").isString())
    {
        m_frameName = config.find("frame_name").asString();
    }
    else
    {
        m_frameName = m_sensorName;
        yCWarning(IMUBOSCH_BNO055) << "Parameter \"frame_name\" not set. Using the same value as \"sensor_name\" for this parameter.";
    }

    return PeriodicThread::start();
}

bool BoschIMU::close()
{
    yCTrace(IMUBOSCH_BNO055);
    //stop the thread
    PeriodicThread::stop();
    return true;
}

bool BoschIMU::checkReadResponse(unsigned char* response)
{
    if(response[0] == (unsigned char) REPLY_HEAD)
    {
        return true;
    }

    if(response[0] == (unsigned char) ERROR_HEAD)
    {
        if(response[1] != REGISTER_NOT_READY)   // if error is 0x07, do not print error messages
        {
            yCError(IMUBOSCH_BNO055,
                    "Inertial sensor didn't understand the command. \n\
                    If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
        }
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yCError(IMUBOSCH_BNO055,
            "Received unknown response message. \n\
            If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
    dropGarbage();
    readSysError();
    return false;
}

bool BoschIMU::checkWriteResponse(unsigned char* response)
{
    if(response[0] == (unsigned char) ERROR_HEAD)
    {
        if(response[1] == (unsigned char) WRITE_SUCC)
        {
            return true;
        }
        if(response[1] != REGISTER_NOT_READY)   // if error is 0x07, do not print error messages
        {
            yCError(IMUBOSCH_BNO055,
                    "Inertial sensor didn't understand the command. \n\
                    If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
        }
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yCError(IMUBOSCH_BNO055,
            "Received unknown response message. \n\
            If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
    dropGarbage();
    readSysError();
    return false;
}

bool BoschIMU::sendReadCommandSer(unsigned char register_add, int len, unsigned char* buf, std::string comment)
{
    int command_len;
    int nbytes_w;

    //
    //   Create a READ message
    //
    bool success = false;
    for(int trials=0; (trials<3) && (success==false); trials++)
    {
        totMessagesRead++;
        command_len = 4;
        command[0]= START_BYTE;     // start byte
        command[1]= READ_CMD;       // read operation
        command[2]= register_add;   // register to read
        command[3]= len;            // length in bytes

//         yCTrace(IMUBOSCH_BNO055, "> READ_COMMAND: %s ... ", comment.c_str());
//         yCTrace(IMUBOSCH_BNO055, "Command is:");
//         printBuffer(command, command_len);

        nbytes_w = ::write(fd, (void*)command, command_len);

        if(nbytes_w != command_len)
        {
            yCError(IMUBOSCH_BNO055) << "Cannot correctly send the message: " << comment;
            // DO NOT return here. If something was sent, then the imu will reply with a message
            // even an error message. I have to parse it before proceeding and not leave garbage behind.
        }
        // Read the write reply
        memset(buf, 0x00, 20);
        int readbytes = readBytes(buf, RESP_HEADER_SIZE);
        if(readbytes != RESP_HEADER_SIZE)
        {
            yCError(IMUBOSCH_BNO055, "Expected %d bytes, read %d instead", RESP_HEADER_SIZE, readbytes);
            success = false;
        }
        else if(!checkReadResponse(buf))
        {
            success = false;
            yarp::os::SystemClock::delaySystem(0.002);
        }
        else
        {
            success = true;
//             yCTrace(IMUBOSCH_BNO055, "> SUCCESS!"); fflush(stdout);

            // Read the data payload
            readBytes(&buf[2], (int) buf[1]);
//             yCTrace(IMUBOSCH_BNO055, "\tReply is:");
//             printBuffer(buf, buf[1]+2);
//             yCTrace(IMUBOSCH_BNO055, "***************");
        }
    }
//     if(!success)
//         yCError(IMUBOSCH_BNO055, "> FAILED reading %s!", comment.c_str());
    return success;
}

bool BoschIMU::sendWriteCommandSer(unsigned char register_add, int len, unsigned char* cmd, std::string comment)
{
    int command_len = 4+len;
    int nbytes_w;

    command[0]= START_BYTE;     // start byte
    command[1]= WRITE_CMD;      // read operation
    command[2]= register_add;   // operation mode register
    command[3]= (unsigned char) len;     // length 1 byte
    for (int i = 0; i < len; i++) {
        command[4 + i] = cmd[i]; // data
    }

//     yCTrace(IMUBOSCH_BNO055, "> WRITE_COMMAND:  %s ... ", comment.c_str());
//     yCTrace(IMUBOSCH_BNO055, "Command is:");
//     printBuffer(command, command_len);

    nbytes_w = ::write(fd, (void*)command, command_len);
    if(nbytes_w != command_len)
    {
        yCError(IMUBOSCH_BNO055) << "Cannot correctly send the message: " << comment;
        // DO NOT return here. If something was sent, then the imu will reply with a message
        // even an error message. I have to parse it before proceeding and not leave garbage behind.
    }

    // Read the write reply
    memset(response, 0x00, 20);
    readBytes(response, 2);
    if(!checkWriteResponse(response))
    {
//         yCTrace(IMUBOSCH_BNO055, "> FAILED!"); fflush(stdout);
        yCError(IMUBOSCH_BNO055) << "FAILED writing " << comment;
        return false;
    }
//     yCTrace(IMUBOSCH_BNO055, "> SUCCESS!"); fflush(stdout);
//     yCTrace(IMUBOSCH_BNO055, "\tReply is:");
//     printBuffer(response, 2);
//     yCTrace(IMUBOSCH_BNO055, "***************");
    return true;
}

int BoschIMU::readBytes(unsigned char* buffer, int bytes)
{
    int r = 0;
    int bytesRead = 0;
    do
    {
        r = ::read(fd, (void*)&buffer[bytesRead], 1);
        if (r > 0) {
            bytesRead += r;
        }
    }
    while(r!=0 && bytesRead < bytes);

    return bytesRead;
}

void BoschIMU::dropGarbage()
{
    char byte;
    while( (::read(fd,  (void*) &byte, 1) > 0 ))
    {
//         yCTrace(IMUBOSCH_BNO055, "Dropping byte 0x%02X", byte);
    }
    return;
}

void BoschIMU::printBuffer(unsigned char* buffer, int length)
{
    for (int i = 0; i < length; i++) {
        printf("\t0x%02X ", buffer[i]);
    }
    printf("\n");
}

void BoschIMU::readSysError()
{
    // avoid recursive error check
    if (checkError) {
        return;
    }

    checkError = true;
    yarp::os::SystemClock::delaySystem(0.002);
    if(!sendReadCommandSer(REG_SYS_STATUS, 1, response, "Read SYS_STATUS register") )
    {
        yCError(IMUBOSCH_BNO055)  << "@ line " << __LINE__;
    }

    if(!sendReadCommandSer(REG_SYS_ERR, 1, response, "Read SYS_ERR register") )
    {
        yCError(IMUBOSCH_BNO055)  << "@ line " << __LINE__;
    }
    checkError = false;
    return;
}

bool BoschIMU::sendAndVerifyCommandSer(unsigned char register_add, int len, unsigned char* cmd, std::string comment)
{
    uint8_t attempts=0;
    bool ret;
    do
    {
      ret=sendWriteCommandSer(register_add, len, cmd, comment);
      attempts++;
    }while((attempts<= ATTEMPTS_NUM_OF_SEND_CONFIG_CMD) && (ret==false));

    return(ret);
}

bool BoschIMU::sendReadCommandI2c(unsigned char register_add, int len, unsigned char* buf, std::string comment)
{
    if (i2c_smbus_read_i2c_block_data(fd, register_add, len, buf) < 0)
    {
        yCError(IMUBOSCH_BNO055) << "Cannot correctly send the message: " << comment;
        return false;
    }
    return true;
}

bool BoschIMU::threadInit()
{
    if (i2c_flag)
    {
        int trials = 0;
        // Make sure we have the right device
        while (i2c_smbus_read_byte_data(fd, REG_CHIP_ID) != BNO055_ID)
        {
            if (trials == 10)
            {
                yCError(IMUBOSCH_BNO055) << "Wrong device on the bus, it is not BNO055";
                return false;
            }
            yarp::os::Time::delay(0.1);
            trials++;

        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

        // Set the device in config mode
        if (i2c_smbus_write_byte_data(fd, REG_OP_MODE, CONFIG_MODE) < 0)
        {
            yCError(IMUBOSCH_BNO055) << "Unable to set the Config mode";
            return false;
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

        if (i2c_smbus_write_byte_data(fd, REG_SYS_TRIGGER, TRIG_EXT_CLK_SEL) < 0)
        {
            yCError(IMUBOSCH_BNO055) << "Unable to set external clock";
            return false;
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

        // Perform any required configuration


        if (i2c_smbus_write_byte_data(fd, REG_PAGE_ID, 0x00) < 0)
        {
            yCError(IMUBOSCH_BNO055) << "Unable to set the page ID";
            return false;
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);
        // Set the device into operative mode

        if (i2c_smbus_write_byte_data(fd, REG_OP_MODE, NDOF_MODE) < 0)
        {
            yCError(IMUBOSCH_BNO055) << "Unable to set the Operative mode";
            return false;
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);
    }
    else
    {
        unsigned char msg;
        timeLastReport = yarp::os::SystemClock::nowSystem();

        msg = 0x00;
        if(!sendAndVerifyCommandSer(REG_PAGE_ID, 1, &msg, "PAGE_ID") )
        {
            yCError(IMUBOSCH_BNO055) << "Set page id 0 failed";
            return(false);
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

    //Removed because useless
        ///////////////////////////////////////
        //
        //      Set power mode
        //
        ///////////////////////////////////////
    //     msg = 0x00;
    //     if(!sendAndVerifyCommand(REG_POWER_MODE, 1, &msg, "Set power mode") )
    //     {
    //          yCError(IMUBOSCH_BNO055)  << "Set power mode failed";
    //          return(false);
    //     }
    //
    //     yarp::os::SystemClock::delaySystem(SWITCHING_TIME);


        ///////////////////////////////////////
        //
        //  Set the device in config mode
        //
        ///////////////////////////////////////

        msg = CONFIG_MODE;
        if(!sendAndVerifyCommandSer(REG_OP_MODE, 1, &msg, "Set config mode") )
        {
            yCError(IMUBOSCH_BNO055)  << "Set config mode failed";
            return(false);
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);


        ///////////////////////////////////////
        //
        //     Set external clock
        //
        ///////////////////////////////////////

        msg = TRIG_EXT_CLK_SEL;
        if(!sendAndVerifyCommandSer(REG_SYS_TRIGGER, 1, &msg, "Set external clock") )
        {
            yCError(IMUBOSCH_BNO055) << "Set external clock failed";
            return(false);
        }
        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

        ///////////////////////////////////////
        //
        // Perform any required configuration
        //
        ///////////////////////////////////////



        /// TODO: meas units, offset and so on ...

        ///////////////////////////////////////
        //
        // Set the device into operative mode
        //
        ///////////////////////////////////////

        msg = NDOF_MODE;
        if(!sendAndVerifyCommandSer(REG_OP_MODE, 1, &msg, "Set config NDOF_MODE") )
        {
            yCError(IMUBOSCH_BNO055) << "Set config NDOF_MODE failed";
            return false;
        }

        yarp::os::SystemClock::delaySystem(SWITCHING_TIME);
    }

    // Do a first read procedure to verify everything is fine.
    // In case the device fails to read, stop it and quit
    for(int i=0; i<10; i++)
    {
        // read data from IMU
        run();
        if (dataIsValid) {
            break;
        } else {
            yarp::os::SystemClock::delaySystem(0.01);
        }
    }

    if(!dataIsValid)
    {
        yCError(IMUBOSCH_BNO055) << "First read from the device failed, check everything is fine and retry";
        return false;
    }

    return true;
}

void BoschIMU::run()
{
    m_timeStamp = yarp::os::SystemClock::nowSystem();

    int16_t raw_data[16];

    // In order to avoid zeros when a single read from a sensor is missing,
    // initialize the new measure to be equal to the previous one
    data_tmp = data;


    if (!(this->*readFunc)(REG_ACC_DATA, 32, response, "Read all"))
    {
        yCError(IMUBOSCH_BNO055) << "Failed to read all the data";
        errs++;
        dataIsValid = false;
        return;
    }
    else
    {
        // Correctly construct int16 data
        for(int i=0; i<16; i++)
        {
            raw_data[i] = response[responseOffset+1+i*2] << 8 | response[responseOffset+i*2];
        }

        // Get quaternion
        quaternion_tmp = quaternion;
        quaternion_tmp.w() = ((double)raw_data[12]) / (2 << 13);
        quaternion_tmp.x() = ((double)raw_data[13]) / (2 << 13);
        quaternion_tmp.y() = ((double)raw_data[14]) / (2 << 13);
        quaternion_tmp.z() = ((double)raw_data[15]) / (2 << 13);

        // Convert to RPY angles
        RPY_angle.resize(3);

        // Check quaternion values are meaningful. The aim of this check is simply
        // to verify values are not garbage.
        // Ideally the correct check is that quaternion.abs ~= 1, but to avoid
        // calling a sqrt every cicle only for a rough estimate, the check here
        // is that the self product is nearly 1
        double sum_squared = quaternion_tmp.w() * quaternion_tmp.w() +
                             quaternion_tmp.x() * quaternion_tmp.x() +
                             quaternion_tmp.y() * quaternion_tmp.y() +
                             quaternion_tmp.z() * quaternion_tmp.z();

        if( (sum_squared < 0.9) || (sum_squared > 1.2) )
        {
            dataIsValid = false;
            return;
        }

        dataIsValid = true;
        RPY_angle   = yarp::math::dcm2rpy(quaternion.toRotationMatrix4x4());
        data_tmp[0] = RPY_angle[0] * 180 / M_PI;
        data_tmp[1] = RPY_angle[1] * 180 / M_PI;
        data_tmp[2] = RPY_angle[2] * 180 / M_PI;

        // Fill in accel values
        data_tmp[3] = (double)raw_data[0] / 100.0;
        data_tmp[4] = (double)raw_data[1] / 100.0;
        data_tmp[5] = (double)raw_data[2] / 100.0;


        // Fill in Gyro values
        data_tmp[6] = (double)raw_data[6] / 16.0;
        data_tmp[7] = (double)raw_data[7] / 16.0;
        data_tmp[8] = (double)raw_data[8] / 16.0;

        // Fill in Magnetometer values
        data_tmp[9]  = (double)raw_data[3] / 16.0;
        data_tmp[10] = (double)raw_data[4] / 16.0;
        data_tmp[11] = (double)raw_data[5] / 16.0;
    }

    // Protect only this section in order to avoid slow race conditions when gathering this data
    {
        std::lock_guard<std::mutex> guard(mutex);
        data       = data_tmp;
        quaternion = quaternion_tmp;
    }

    if (m_timeStamp > timeLastReport + TIME_REPORT_INTERVAL) {
        // if almost 1 errors occourred in last interval, then print report
        if(errs != 0)
        {
            yCDebug(IMUBOSCH_BNO055, "Periodic error report of last %d sec:", TIME_REPORT_INTERVAL);
            yCDebug(IMUBOSCH_BNO055, "\t errors while reading data: %d", errs);
        }

        errs = 0;
        timeLastReport=m_timeStamp;
    }
}

bool BoschIMU::read(yarp::sig::Vector &out)
{
    std::lock_guard<std::mutex> guard(mutex);
    out.resize(nChannels);
    out.zero();

    out = data;
    if(nChannels == 16)
    {
        out[12] = quaternion.w();
        out[13] = quaternion.x();
        out[14] = quaternion.y();
        out[15] = quaternion.z();
    }

    return dataIsValid;
}

bool BoschIMU::getChannels(int *nc)
{
    *nc = nChannels;
    return true;
}

bool BoschIMU::calibrate(int ch, double v)
{
    // TODO: start a calib procedure in which the calib status register is read
    // until all sensors are calibrated (0xFFFF). Then the offsets are saved
    // into memory for the next run.
    // This procedure should be abortable by CTRL+C
    return false;
}


yarp::dev::MAS_status BoschIMU::genericGetStatus(size_t sens_index) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return yarp::dev::MAS_status::MAS_ERROR;
    }

    return yarp::dev::MAS_status::MAS_OK;
}

bool BoschIMU::genericGetSensorName(size_t sens_index, string& name) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    name = m_sensorName;
    return true;
}

bool BoschIMU::genericGetFrameName(size_t sens_index, string& frameName) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    frameName = m_frameName;
    return true;

}

size_t BoschIMU::getNrOfThreeAxisLinearAccelerometers() const
{
    return 1;
}


yarp::dev::MAS_status BoschIMU::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool BoschIMU::getThreeAxisLinearAccelerometerName(size_t sens_index, string& name) const
{
    return genericGetSensorName(sens_index, name);
}

bool BoschIMU::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, string& frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool BoschIMU::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    out.resize(3);
    std::lock_guard<std::mutex> guard(mutex);
    out[0] = data[3];
    out[1] = data[4];
    out[2] = data[5];

    timestamp = m_timeStamp;
    return true;
}


size_t BoschIMU::getNrOfThreeAxisGyroscopes() const
{
    return 1;
}


yarp::dev::MAS_status BoschIMU::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool BoschIMU::getThreeAxisGyroscopeName(size_t sens_index, string& name) const
{
    return genericGetSensorName(sens_index, name);
}

bool BoschIMU::getThreeAxisGyroscopeFrameName(size_t sens_index, string& frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool BoschIMU::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    out.resize(3);
    std::lock_guard<std::mutex> guard(mutex);
    out[0] = data[6];
    out[1] = data[7];
    out[2] = data[8];

    timestamp = m_timeStamp;
    return true;
}

size_t BoschIMU::getNrOfOrientationSensors() const
{
    return 1;
}

yarp::dev::MAS_status BoschIMU::getOrientationSensorStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool BoschIMU::getOrientationSensorName(size_t sens_index, string& name) const
{
    return genericGetSensorName(sens_index, name);
}

bool BoschIMU::getOrientationSensorFrameName(size_t sens_index, string& frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool BoschIMU::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    rpy.resize(3);
    std::lock_guard<std::mutex> guard(mutex);
    rpy[0] = data[0];
    rpy[1] = data[1];
    rpy[2] = data[2];

    timestamp = m_timeStamp;
    return true;
}

size_t BoschIMU::getNrOfThreeAxisMagnetometers() const
{
    return 1;
}

yarp::dev::MAS_status BoschIMU::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool BoschIMU::getThreeAxisMagnetometerName(size_t sens_index, string& name) const
{
    return genericGetSensorName(sens_index, name);
}

bool BoschIMU::getThreeAxisMagnetometerFrameName(size_t sens_index, string& frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool BoschIMU::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index != 0)
    {
        yCError(IMUBOSCH_BNO055) << "sens_index must be equal to 0, since there is only one sensor in consideration";
        return false;
    }

    out.resize(3);
    std::lock_guard<std::mutex> guard(mutex);
    // The unit measure of Bosch BNO055 is uT
    out[0] = data[9] / 1000000;
    out[1] = data[10]/ 1000000;
    out[2] = data[11]/ 1000000;

    timestamp = m_timeStamp;
    return true;
}


void BoschIMU::threadRelease()
{
    yCTrace(IMUBOSCH_BNO055, "Thread released");
    //TBD write more meaningful report
//    for(unsigned int i=0; i<errorCounter.size(); i++)
//        yCTrace(IMUBOSCH_BNO055, "Error type %d, counter is %d", i, (int)errorCounter[i]);
//    yCTrace(IMUBOSCH_BNO055, "On overall read operations of %ld", totMessagesRead);
    ::close(fd);
}
