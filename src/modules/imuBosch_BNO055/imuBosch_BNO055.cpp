// Copyright: (C) 2016 iCub Facility
// Authors: Alberto Cardellino <alberto.cardellino@iit.it>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.


#include <stdlib.h>
#include <unistd.h>
#include <termios.h> // terminal io (serial port) interface
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <math.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Log.h>
#include <yarp/math/Math.h>

#include "imuBosch_BNO055.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

BoschIMU::BoschIMU():   RateThread(20), mutex(1),
                        checkError(false)
{
    data.resize(12);
    data.zero();
    errorCounter.resize(11);
    errorCounter.zero();
    totMessagesRead = 0;
    quaternion.resize(4);
    nChannels = 12;
}

BoschIMU::~BoschIMU() { }


bool BoschIMU::open(yarp::os::Searchable& config)
{
    //debug
    yInfo("\nParameters are:\n\t%s\n", config.toString().c_str());

    if(!config.check("comport"))
    {
        yError() << "Param 'comport' not found";
        return false;
    }

    int period = config.check("period",Value(10),"Thread period in ms").asInt();
    setRate(period);

    nChannels = config.check("channels", Value(12)).asInt();

     printf("\n\nSerial opening %s\n\n\n", config.find("comport").toString().c_str());
    fd_ser = ::open(config.find("comport").toString().c_str(), O_RDWR | O_NOCTTY );
    if (fd_ser < 0) {
        printf("can't open %s, %s\n", config.find("comport").toString().c_str(), strerror(errno));
        return false;
    }

    //Get the current options for the port...
    struct termios options;
    tcgetattr(fd_ser, &options);

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

    printf("\n*** NOT BLOCKING READ!! ***\n");
    options.c_cc[VMIN]  = 0;   // block reading until RX x characters. If x = 0, it is non-blocking.
    options.c_cc[VTIME] = 2;   // Inter-Character Timer -- i.e. timeout= x*.1 s

    //Set local mode and enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    tcflush(fd_ser, TCIOFLUSH);

    //Set the new options for the port...
    if ( tcsetattr(fd_ser, TCSANOW, &options) != 0)
    {
        printf("Configuring comport failed\n");
        return false;
    }

    if(!RateThread::start())
        return false;

    return true;
}

bool BoschIMU::close()
{
    yTrace();
    //stop the thread
    RateThread::stop();
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
            yError("Bosch BNO055 IMU - Received error response (0x%02X) - code 0x%02X", response[0], response[1]);
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yError("Bosch BNO055 IMU - Received unknown read response: 0x%02x 0x%02x. This should not happen!!", response[0], response[1]);
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
        yError("Bosch BNO055 IMU - Received error response (0x%02X) - code 0x%02X", response[0], response[1]);
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yError("Bosch BNO055 IMU - Received unknown write response: 0x%02x 0x%02x. This should not happen!!", response[0], response[1]);
    dropGarbage();
    readSysError();
    return false;
}

bool BoschIMU::sendReadCommand(unsigned char register_add, int len, unsigned char* buf, std::string comment)
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

//         printf("> READ_COMMAND: %s ... ", comment.c_str());
//         printf("\nCommand is:\n");
//         printBuffer(command, command_len);

        nbytes_w = ::write(fd_ser, (void*)command, command_len);

        if(nbytes_w != command_len)
        {
            yError() << "BoschIMU device cannot correctly send the message: " << comment;
            // DO NOT return here. If something was sent, then the imu will reply with a message
            // even an error message. I have to parse it before proceeding and not leave garbage behind.
        }
        // Read the write reply
        memset(buf, 0x00, 20);
        readBytes(buf, 2);
        if(!checkReadResponse(buf))
        {
            success = false;
            yarp::os::Time::delay(0.002);
        }
        else
        {
            success = true;
//             printf("> SUCCESS!\n"); fflush(stdout);

            // Read the data payload
            readBytes(&buf[2], (int) buf[1]);
//             printf("\tReply is:\n");
//             printBuffer(buf, buf[1]+2);
//             printf("***************\n");
        }
    }
    if(!success)
        yError("> FAILED reading %s!\n", comment.c_str());
    return success;
}

bool BoschIMU::sendWriteCommand(unsigned char register_add, int len, unsigned char* cmd, std::string comment)
{
    int command_len = 4+len;
    int nbytes_w;

    command[0]= START_BYTE;     // start byte
    command[1]= WRITE_CMD;      // read operation
    command[2]= register_add;   // operation mode register
    command[3]= (unsigned char) len;     // length 1 byte
    for(int i=0; i<len; i++)
        command[4+i] = cmd[i];  // data

//     printf("> WRITE_COMMAND:  %s ... ", comment.c_str());
//     printf("\nCommand is:\n");
//     printBuffer(command, command_len);

    nbytes_w = ::write(fd_ser, (void*)command, command_len);
    if(nbytes_w != command_len)
    {
        yError() << "BoschIMU device cannot correctly send the message: " << comment;
        // DO NOT return here. If something was sent, then the imu will reply with a message
        // even an error message. I have to parse it before proceeding and not leave garbage behind.
    }

    // Read the write reply
    memset(response, 0x00, 20);
    readBytes(response, 2);
    if(!checkWriteResponse(response))
    {
//         printf("> FAILED!\n"); fflush(stdout);
        yError() << "FAILED writing " << comment;
        return false;
    }
//     printf("> SUCCESS!\n"); fflush(stdout);
//     printf("\tReply is:\n");
//     printBuffer(response, 2);
//     printf("***************\n");
    return true;
}

int BoschIMU::readBytes(unsigned char* buffer, int bytes)
{
    int r = 0;
    int bytesRead = 0;
    do
    {
        r = ::read(fd_ser, (void*)&buffer[bytesRead], 1);
        if(r > 0)
            bytesRead += r;
    }
    while(r!=0 && bytesRead < bytes);
    if(bytesRead != bytes)
        yError("Expected %d bytes, read %d instead\n", bytes, bytesRead);

    return bytesRead;
}

void BoschIMU::dropGarbage()
{
    char byte;
    while( (::read(fd_ser,  (void*) &byte, 1) > 0 ))
    {
//         printf("Dropping byte 0x%02X \n", byte);
    }
    return;
}

void BoschIMU::printBuffer(unsigned char* buffer, int length)
{
    for(int i=0; i< length; i++)
        printf("\t0x%02X ", buffer[i]);
    printf("\n");
}

void BoschIMU::readSysError()
{
    // avoid recursive error check
    if(checkError)
        return;

    checkError = true;
    yarp::os::Time::delay(0.002);
    if(!sendReadCommand(REG_SYS_STATUS, 1, response, "Read SYS_STATUS register") )
    {
        yError()  << "@ line " << __LINE__;
    }

    if(!sendReadCommand(REG_SYS_ERR, 1, response, "Read SYS_ERR register") )
    {
        yError()  << "@ line " << __LINE__;
    }
    checkError = false;
    return;
}

bool BoschIMU::threadInit()
{
    unsigned char msg;

    msg = 0x00;
    if(!sendWriteCommand(REG_PAGE_ID, 1, &msg, "PAGE_ID") )
    {
        yError()  << "@ line " << __LINE__;
    }

    yarp::os::Time::delay(SWITCHING_TIME);

    ///////////////////////////////////////
    //
    //      Set power mode
    //
    ///////////////////////////////////////
    msg = 0x00;
    if(!sendWriteCommand(REG_POWER_MODE, 1, &msg, "Set power mode") )
    {
        yError()  << "@ line " << __LINE__;
    }

    yarp::os::Time::delay(SWITCHING_TIME);

    ///////////////////////////////////////
    //
    //      Read power mode
    //
    ///////////////////////////////////////

    if(!sendReadCommand(REG_POWER_MODE, 1, response, "Read power mode register") )
    {
        yError()  << "@ line " << __LINE__;
    }

    yarp::os::Time::delay(SWITCHING_TIME);

    ///////////////////////////////////////
    //
    //  Set the device in config mode
    //
    ///////////////////////////////////////

    msg = CONFIG_MODE;
    if(!sendWriteCommand(REG_OP_MODE, 1, &msg, "Set config mode") )
    {
        yError()  << "@ line " << __LINE__;
    }

    yarp::os::Time::delay(SWITCHING_TIME);

    ///////////////////////////////////////
    //
    // Read back the config and verify
    //
    ///////////////////////////////////////

    if(!sendReadCommand(REG_OP_MODE, 1, response, "Read config register") )
    {
        yError()  << "@ line " << __LINE__;
    }

    ///////////////////////////////////////
    //
    //     Set external clock
    //
    ///////////////////////////////////////

    msg = TRIG_EXT_CLK_SEL;
    if(!sendWriteCommand(REG_SYS_TRIGGER, 1, &msg, "Set external clock") )
    {
        yError()  << "@ line " << __LINE__;
    }
    yarp::os::Time::delay(SWITCHING_TIME);

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
    if(!sendWriteCommand(REG_OP_MODE, 1, &msg, "Set config NDOF_MODE") )
    {
        yError()  << "@ line " << __LINE__;
    }

    yarp::os::Time::delay(SWITCHING_TIME);

    //
    // TODO: read back the config and verify
    //
    if(!sendReadCommand(REG_OP_MODE, 1, response, "Read config register") )
    {
        yError()  << "@ line " << __LINE__;
    }
    if(response[2] != msg)
    {
        yError("Config mode set was 0x%02X but read 0X%02X instead", msg, response[2]);
        return false;
    }
    yarp::os::Time::delay(SWITCHING_TIME);
    return true;
}

void BoschIMU::run()
{
    timeStamp = yarp::os::Time::now();

    int16_t raw_data[4];
//     void *tmp = (void*) &response[2];
//     raw_data = static_cast<int16_t *> (tmp);

    // TODO: how to optimally protect only code filling the data vector?

    mutex.wait();

    ///////////////////////////////////////
    //
    //      Read calibration status
    //
    ///////////////////////////////////////

    if(!sendReadCommand(REG_CALIB_STATUS, 1, response, "Read calib status") )
    {
        yError()  << "@ line " << __LINE__;
    }


    data.zero();
    ///////////////////////////////////////////
    //
    // Read ID
    //
    ///////////////////////////////////////////

    if(!sendReadCommand(REG_CHIP_ID, 1, response, "Read CHIP ID register") )
    {
        yError()  << "@ line " << __LINE__;
    }

    ///////////////////////////////////////////
    //
    //  Read RPY values  --> no!! There are bad spikes on yaw values!!
    //  Read quaternion and convert back to RPY afterwards
    //
    ///////////////////////////////////////////

//     if(!sendReadCommand(REG_RPY_DATA, 6, response, "Read RPY ... ") )
//     {
//         yError()  << "@ line " << __LINE__;
//     }

//     data[2] = (double) raw_data[0]/16.0;
//     data[0] = (double) raw_data[1]/16.0;
//     data[1] = (double) raw_data[2]/16.0;
//     yDebug() << "RPY x: " << data[0] << "y: " << data[1] << "z: " << data[2];

    ///////////////////////////////////////////
    //
    //      Read accel values
    //
    ///////////////////////////////////////////

    if(!sendReadCommand(REG_ACC_DATA, 6, response, "Read accelerations ... ") )
    {
        yError()  << "@ line " << __LINE__;
    }

    // Manually compose the data to safely handling endianess
    raw_data[0] = response[3] << 8 | response[2];
    raw_data[1] = response[5] << 8 | response[4];
    raw_data[2] = response[7] << 8 | response[6];
    data[3] = (double) raw_data[0]/100.0;
    data[4] = (double) raw_data[1]/100.0;
    data[5] = (double) raw_data[2]/100.0;

    ///////////////////////////////////////////
    //
    //      Read Gyro values
    //
    ///////////////////////////////////////////

    if(!sendReadCommand(REG_GYRO_DATA, 6, response, "Read Gyros ... ") )
    {
        yError()  << "@ line " << __LINE__;
    }

    // Manually compose the data to safely handling endianess
    raw_data[0] = response[3] << 8 | response[2];
    raw_data[1] = response[5] << 8 | response[4];
    raw_data[2] = response[7] << 8 | response[6];
    data[6] = (double) raw_data[0]/16.0;
    data[7] = (double) raw_data[1]/16.0;
    data[8] = (double) raw_data[2]/16.0;
//     yDebug() << "Gyro x: " << data[6] << "y: " << data[7] << "z: " << data[8];

    ///////////////////////////////////////////
    //
    //      Read Magnetometer values
    //
    ///////////////////////////////////////////

    if(!sendReadCommand(REG_MAGN_DATA, 6, response, "Read Magnetometer ... ") )
    {
        yError()  << "@ line " << __LINE__;
    }

    // Manually compose the data to safely handling endianess
    raw_data[0] = response[3] << 8 | response[2];
    raw_data[1] = response[5] << 8 | response[4];
    raw_data[2] = response[7] << 8 | response[6];
    data[ 9] = (double) raw_data[0]/16.0;
    data[10] = (double) raw_data[1]/16.0;
    data[11] = (double) raw_data[2]/16.0;
//     yDebug() << "Magn x: " << data[9] << "y: " << data[10] << "z: " << data[11];

    ///////////////////////////////////////////
    //
    //      Read Quaternion
    //
    ///////////////////////////////////////////

    if(!sendReadCommand(REG_QUATERN_DATA, 8, response, "Read quaternion ... ") )
    {
        yError()  << "@ line " << __LINE__;
    }

    quaternion.zero();
    // Manually compose the data to safely handling endianess
    raw_data[0] = response[3] << 8 | response[2];
    raw_data[1] = response[5] << 8 | response[4];
    raw_data[2] = response[7] << 8 | response[6];
    raw_data[3] = response[9] << 8 | response[8];
    
    quaternion[0] = ((double) raw_data[1])/(2<<13);
    quaternion[1] = ((double) raw_data[2])/(2<<13);
    quaternion[2] = ((double) raw_data[3])/(2<<13);
    quaternion[3] = ((double) raw_data[0])/(2<<13);

    RPY_angle.resize(3);
    RPY_angle = yarp::math::dcm2rpy(yarp::math::quat2dcm(quaternion));
    data[0] = RPY_angle[0] * 180/M_PI;
    data[1] = RPY_angle[1] * 180/M_PI;
    data[2] = RPY_angle[2] * 180/M_PI;

    // If 100ms have passed since the last received message
    if (timeStamp+0.1 < yarp::os::Time::now())
    {
//         status=TIMEOUT;
    }
    mutex.post();
}

bool BoschIMU::read(yarp::sig::Vector &out)
{
//     yTrace();
    mutex.wait();
    out.resize(nChannels);
    out.zero();

    out = data;
    if(nChannels == 16)
    {
        out[12] = quaternion[0];
        out[13] = quaternion[1];
        out[14] = quaternion[2];
        out[15] = quaternion[3];
    }

    mutex.post();
    return true;
};

bool BoschIMU::getChannels(int *nc)
{
    *nc = nChannels;
    return true;
};

bool BoschIMU::calibrate(int ch, double v)
{
    // TODO: start a calib procedure in which the calib status register is read
    // until all sensors are calibrated (0xFFFF). Then the offsets are saved
    // into memory for the next run.
    // This procedure should be abortable by CTRL+C
    return false;
};

void BoschIMU::threadRelease()
{
    yTrace("BoschIMU Thread released\n");
    for(unsigned int i=0; i<errorCounter.size(); i++)
        printf("Error type %d, counter is %d\n", i, (int)errorCounter[i]);
    printf("On overall read operations of %ld\n", totMessagesRead);
    ::close(fd_ser);
}

