// Copyright: (C) 2016 iCub Facility
// Authors: Alberto Cardellino <alberto.cardellino@iit.it>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.


#include <cstdlib>
#include <unistd.h>
#include <termios.h> // terminal io (serial port) interface
#include <fcntl.h>   // File control definitions
#include <cerrno>   // Error number definitions
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <cmath>

#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Log.h>
#include <yarp/math/Math.h>
#include <yarp/os/LockGuard.h>

#include "imuBosch_BNO055.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

BoschIMU::BoschIMU() : RateThread(20), checkError(false)
{
    data.resize(12);
    data.zero();
    data_tmp.resize(12);
    data_tmp.zero();
    errorCounter.resize(11);
    errorCounter.zero();
    totMessagesRead = 0;
    nChannels = 12;
    errs.acceError = 0;
    errs.gyroError = 0;
    errs.magnError = 0;
    errs.quatError = 0;
}

BoschIMU::~BoschIMU() { }


bool BoschIMU::open(yarp::os::Searchable& config)
{
    //debug
    yTrace("Parameters are:\n\t%s", config.toString().c_str());

    if(!config.check("comport"))
    {
        yError() << "Param 'comport' not found";
        return false;
    }

    int period = config.check("period",Value(10),"Thread period in ms").asInt();
    setRate(period);

    nChannels = config.check("channels", Value(12)).asInt();

    fd_ser = ::open(config.find("comport").toString().c_str(), O_RDWR | O_NOCTTY );
    if (fd_ser < 0) {
        yError("can't open %s, %s", config.find("comport").toString().c_str(), strerror(errno));
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

    // SET NOT BLOCKING READ
    options.c_cc[VMIN]  = 0;   // block reading until RX x characters. If x = 0, it is non-blocking.
    options.c_cc[VTIME] = 2;   // Inter-Character Timer -- i.e. timeout= x*.1 s

    //Set local mode and enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    tcflush(fd_ser, TCIOFLUSH);

    //Set the new options for the port...
    if ( tcsetattr(fd_ser, TCSANOW, &options) != 0)
    {
        yError("Configuring comport failed");
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
        {
            yError("Bosch BNO055 IMU - Inertial sensor didn't understand the command. \n\
            If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
        }
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yError("Bosch BNO055 IMU - Received unknown response message. \n\
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
            yError("Bosch BNO055 IMU - Inertial sensor didn't understand the command. \n\
            If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
        }
        errorCounter[response[1]]++;
        readSysError();
        return false;
    }

    errorCounter[0]++;
    yError("Bosch BNO055 IMU - Received unknown response message. \n\
            If this error happens more than once in a row, please check serial communication is working fine and it isn't affected by electrical disturbance.");
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
        int readbytes = readBytes(buf, RESP_HEADER_SIZE);
        if(readbytes != RESP_HEADER_SIZE)
        {
            yError("Expected %d bytes, read %d instead\n", RESP_HEADER_SIZE, readbytes);
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
//             printf("> SUCCESS!\n"); fflush(stdout);

            // Read the data payload
            readBytes(&buf[2], (int) buf[1]);
//             printf("\tReply is:\n");
//             printBuffer(buf, buf[1]+2);
//             printf("***************\n");
        }
    }
//     if(!success)
//         yError("> FAILED reading %s!\n", comment.c_str());
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
    yarp::os::SystemClock::delaySystem(0.002);
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

bool BoschIMU::sendAndVerifyCommand(unsigned char register_add, int len, unsigned char* cmd, std::string comment)
{
    uint8_t attempts=0;
    bool ret;
    do
    {
      ret=sendWriteCommand(register_add, len, cmd, comment);
      attempts++;
    }while((attempts<= ATTEMPTS_NUM_OF_SEND_CONFIG_CMD) && (ret==false));

    return(ret);
}

bool BoschIMU::threadInit()
{
    unsigned char msg;
    timeLastReport = yarp::os::SystemClock::nowSystem();

    msg = 0x00;
    if(!sendAndVerifyCommand(REG_PAGE_ID, 1, &msg, "PAGE_ID") )
    {
        yError()  << "BoschIMU: set page id 0 failed";
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
//          yError()  << "BoschIMU: set power mode failed";
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
    if(!sendAndVerifyCommand(REG_OP_MODE, 1, &msg, "Set config mode") )
    {
        yError()  << "BoschIMU: set config mode failed";
        return(false);
    }

    yarp::os::SystemClock::delaySystem(SWITCHING_TIME);


    ///////////////////////////////////////
    //
    //     Set external clock
    //
    ///////////////////////////////////////

    msg = TRIG_EXT_CLK_SEL;
    if(!sendAndVerifyCommand(REG_SYS_TRIGGER, 1, &msg, "Set external clock") )
    {
        yError()  << "BoschIMU: set external clock failed";
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
    if(!sendAndVerifyCommand(REG_OP_MODE, 1, &msg, "Set config NDOF_MODE") )
    {
        yError()  << "BoschIMU: set config NDOF_MODE failed";
    }

    yarp::os::SystemClock::delaySystem(SWITCHING_TIME);

    return true;
}

void BoschIMU::run()
{
    timeStamp = yarp::os::SystemClock::nowSystem();

    int16_t raw_data[4];
//     void *tmp = (void*) &response[2];
//     raw_data = static_cast<int16_t *> (tmp);

    // In order to avoid zeros when a single read from a sensor is missing,
    // initialize the new measure to be equal to the previous one
    data_tmp = data;

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

    if (sendReadCommand(REG_ACC_DATA, 6, response, "Read accelerations")) {
        // Manually compose the data to safely handling endianess
        raw_data[0] = response[3] << 8 | response[2];
        raw_data[1] = response[5] << 8 | response[4];
        raw_data[2] = response[7] << 8 | response[6];
        data_tmp[3] = (double)raw_data[0] / 100.0;
        data_tmp[4] = (double)raw_data[1] / 100.0;
        data_tmp[5] = (double)raw_data[2] / 100.0;
    }
    else {
        errs.acceError++;
    }

    ///////////////////////////////////////////
    //
    //      Read Gyro values
    //
    ///////////////////////////////////////////

    if (sendReadCommand(REG_GYRO_DATA, 6, response, "Read Gyros")) {
        // Manually compose the data to handle endianess safely
        raw_data[0] = response[3] << 8 | response[2];
        raw_data[1] = response[5] << 8 | response[4];
        raw_data[2] = response[7] << 8 | response[6];
        data_tmp[6] = (double)raw_data[0] / 16.0;
        data_tmp[7] = (double)raw_data[1] / 16.0;
        data_tmp[8] = (double)raw_data[2] / 16.0;
        //     yDebug() << "Gyro x: " << data[6] << "y: " << data[7] << "z: " << data[8];
    }
    else {
        errs.gyroError++;
    }

    ///////////////////////////////////////////
    //
    //      Read Magnetometer values
    //
    ///////////////////////////////////////////

    if (sendReadCommand(REG_MAGN_DATA, 6, response, "Read Magnetometer")) {
        // Manually compose the data to safely handling endianess
        raw_data[0]  = response[3] << 8 | response[2];
        raw_data[1]  = response[5] << 8 | response[4];
        raw_data[2]  = response[7] << 8 | response[6];
        data_tmp[9]  = (double)raw_data[0] / 16.0;
        data_tmp[10] = (double)raw_data[1] / 16.0;
        data_tmp[11] = (double)raw_data[2] / 16.0;
        // yDebug() << "Magn x: " << data[9] << "y: " << data[10] << "z: " << data[11];
    }
    else {
        errs.magnError++;
    }

    ///////////////////////////////////////////
    //
    //      Read Quaternion
    //
    ///////////////////////////////////////////

    quaternion_tmp = quaternion;
    if (sendReadCommand(REG_QUATERN_DATA, 8, response, "Read quaternion")) {
        // Manually compose the data to safely handling endianess
        raw_data[0] = response[3] << 8 | response[2];
        raw_data[1] = response[5] << 8 | response[4];
        raw_data[2] = response[7] << 8 | response[6];
        raw_data[3] = response[9] << 8 | response[8];

        quaternion_tmp.w() = ((double)raw_data[0]) / (2 << 13);
        quaternion_tmp.x() = ((double)raw_data[1]) / (2 << 13);
        quaternion_tmp.y() = ((double)raw_data[2]) / (2 << 13);
        quaternion_tmp.z() = ((double)raw_data[3]) / (2 << 13);

        RPY_angle.resize(3);
        RPY_angle   = yarp::math::dcm2rpy(quaternion.toRotationMatrix());
        data_tmp[0] = RPY_angle[0] * 180 / M_PI;
        data_tmp[1] = RPY_angle[1] * 180 / M_PI;
        data_tmp[2] = RPY_angle[2] * 180 / M_PI;
    }
    else {
        errs.quatError++;
    }

    // If 100ms have passed since the last received message
    if (timeStamp+0.1 < yarp::os::SystemClock::nowSystem())
    {
//         status=TIMEOUT;
    }

    // Protect only this section in order to avoid slow race conditions when gathering this data
    {
        LockGuard guard(mutex);
        data       = data_tmp;
        quaternion = quaternion_tmp;
    }

    if (timeStamp > timeLastReport + TIME_REPORT_INTERVAL) {
        // if almost 1 errors occourred in last interval, then print report
        if(errs.acceError + errs.gyroError + errs.magnError + errs.quatError != 0)
        {
            yDebug(" IMUBOSCH periodic error report of last %d sec:", TIME_REPORT_INTERVAL);
            yDebug("\t errors while reading acceleration: %d", errs.acceError);
            yDebug("\t errors while reading gyroscope   : %d", errs.gyroError);
            yDebug("\t errors while reading magnetometer: %d", errs.magnError);
            yDebug("\t errors while reading angles      : %d", errs.quatError);
        }

        errs.acceError = 0;
        errs.gyroError = 0;
        errs.magnError = 0;
        errs.quatError = 0;
        timeLastReport=timeStamp;
    }
}

bool BoschIMU::read(yarp::sig::Vector &out)
{
    LockGuard guard(mutex);
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
    //TBD write more meaningful report
//    for(unsigned int i=0; i<errorCounter.size(); i++)
//        printf("Error type %d, counter is %d\n", i, (int)errorCounter[i]);
//    printf("On overall read operations of %ld\n", totMessagesRead);
    ::close(fd_ser);
}

