/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2008 Giacomo Spigler
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SerialServoBoard.h"

#include <yarp/os/LogComponent.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/ISerialDevice.h>
#include <yarp/dev/PolyDriver.h>

#include <cstdio>
#include <cstdlib>


//TODO: check limits of operation (range of angles)?


using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(SERIALSERVOBOARD, "yarp.devices.SerialServoBoard")
}


bool SerialServoBoard::open(Searchable& config)
{
    if (config.check("help") == true) {
        yCInfo(SERIALSERVOBOARD, "SerialServoBoard Available Options:");
        yCInfo(SERIALSERVOBOARD, " -board NAME, where name is one of ssc32, minissc, pontech_sv203x, mondotronic_smi, parallax, pololu_usb_16servo, picopic");
        yCInfo(SERIALSERVOBOARD, " -comport NAME, where name is COMx on Windows, and /dev/ttySx on Linux");
        yCInfo(SERIALSERVOBOARD, " -baudrate RATE, where RATE is the Board baudrate");
        yCInfo(SERIALSERVOBOARD, " -help shows this help");

        return false;
    }


    char servoboard_[80];

    strcpy(servoboard_, config.check("board", yarp::os::Value("ssc32")).asString().c_str());

    if (strcmp(servoboard_, "ssc32") == 0) {
        servoboard = SSC32;
        move = &movessc32;
    } else if (strcmp(servoboard_, "minissc") == 0) {
        servoboard = MINISSC;
        move = &moveminissc;
    } else if (strcmp(servoboard_, "pontech_sv203x") == 0) {
        servoboard = PONTECHSV203X;
        move = &movepontech;
    } else if (strcmp(servoboard_, "mondotronic_smi") == 0) {
        servoboard = MONDOTRONICSMI;
        move = &movemondotronic;
    } else if (strcmp(servoboard_, "pololu_usb_16servo") == 0) {
        servoboard = POLOLUUSB16;
        move = &movepololu;
    } else if (strcmp(servoboard_, "picopic") == 0) {
        servoboard = PICOPIC;
        move = &movepicopic;
    }


    char comport[80];

    strcpy(comport, config.check("comport", yarp::os::Value("/dev/ttyS0")).asString().c_str());

    int baudrate = config.check("baudrate", yarp::os::Value(38400)).asInt32();

    Property conf;
    // no arguments, use a default
    conf.put("device", "serialport");
    conf.put("comport", comport);
    conf.put("baudrate", baudrate);
    conf.put("rcvenb", 1);
    conf.put("stopbits", 2);
    conf.put("databits", 8);
    conf.put("paritymode", "none");

    dd.open(conf);
    if (!dd.isValid()) {
        yCError(SERIALSERVOBOARD, "Failed to create and configure a device");
        std::exit(1);
    }

    if (!dd.view(serial)) {
        yCError(SERIALSERVOBOARD, "Failed to view device through ISerialDevice interface");
        std::exit(1);
    }


    positions = (double*)malloc(sizeof(double) * 32);
    speeds = (double*)malloc(sizeof(double) * 32);

    return true;
}

bool SerialServoBoard::close()
{
    dd.close();

    free(positions);
    free(speeds);

    return true;
}


bool SerialServoBoard::getAxes(int* ax)
{
    if (servoboard == SSC32) {
        *ax = 32;
    } else if (servoboard == MINISSC) {
        *ax = 8;
    } else if (servoboard == PONTECHSV203X) {
        *ax = 8;
    } else if (servoboard == MONDOTRONICSMI) {
        *ax = 2;
    } else if (servoboard == POLOLUUSB16) {
        *ax = 16;
    } else if (servoboard == PICOPIC) {
        *ax = 20;
    }

    return true;
}


bool SerialServoBoard::positionMove(int j, double ref)
{
    positions[j] = ref;

    return move(j, ref, positions, speeds, serial);
}


bool SerialServoBoard::positionMove(const double* refs)
{
    for (int k = 0; k < 32; k++) {
        this->positionMove(k, refs[k]);
    }

    return true;
}


bool SerialServoBoard::relativeMove(int j, double delta)
{
    this->positionMove(j, positions[j] + delta);

    return true;
}


bool SerialServoBoard::relativeMove(const double* deltas)
{
    for (int k = 0; k < 32; k++) {
        this->positionMove(k, positions[k] + deltas[k]);
    }

    return true;
}


bool SerialServoBoard::checkMotionDone(int j, bool* flag)
{
    //TODO: Q?

    return true;
}


bool SerialServoBoard::checkMotionDone(bool* flag)
{
    //TODO: Q?

    return true;
}


bool SerialServoBoard::setRefSpeed(int j, double sp)
{
    speeds[j] = sp;

    return true;
}


bool SerialServoBoard::setRefSpeeds(const double* spds)
{
    for (int k = 0; k < 32; k++) {
        setRefSpeed(k, spds[k]);
    }

    return true;
}


bool SerialServoBoard::setRefAcceleration(int j, double acc)
{
    return true;
}


bool SerialServoBoard::setRefAccelerations(const double* accs)
{
    return true;
}


bool SerialServoBoard::getRefSpeed(int j, double* ref)
{
    *ref = speeds[j];

    return true;
}


bool SerialServoBoard::getRefSpeeds(double* spds)
{
    for (int k = 0; k < 32; k++) {
        spds[k] = speeds[k];
    }

    return true;
}


bool SerialServoBoard::getRefAcceleration(int j, double* acc)
{
    return true;
}


bool SerialServoBoard::getRefAccelerations(double* accs)
{
    return true;
}


bool SerialServoBoard::stop(int j)
{
    return true;
}


bool SerialServoBoard::stop()
{
    return true;
}

bool SerialServoBoard::positionMove(const int n_joint, const int* joints, const double* refs)
{
    return true;
}
bool SerialServoBoard::relativeMove(const int n_joint, const int* joints, const double* deltas)
{
    return true;
}
bool SerialServoBoard::checkMotionDone(const int n_joint, const int* joints, bool* flags)
{
    return true;
}
bool SerialServoBoard::setRefSpeeds(const int n_joint, const int* joints, const double* spds)
{
    return true;
}
bool SerialServoBoard::setRefAccelerations(const int n_joint, const int* joints, const double* accs)
{
    return true;
}
bool SerialServoBoard::getRefSpeeds(const int n_joint, const int* joints, double* spds)
{
    return true;
}
bool SerialServoBoard::getRefAccelerations(const int n_joint, const int* joints, double* accs)
{
    return true;
}
bool SerialServoBoard::stop(const int n_joint, const int* joints)
{
    return true;
}


bool movessc32(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    int pos = 1500 + round(positions[j] * 11.11);

    Bottle bot;
    char str[80];
    if (FABS(speeds[j]) < 0.1) {
        std::snprintf(str, 80, "#%dP%d\r", j, pos);
    } else {
        int speed = round(speeds[j] * 11.11);

        std::snprintf(str, 80, "#%dP%dS%d\r", j, pos, speed);
    }
    //if(j==0) {
    bot.addString(str);
    serial->send(bot);
    //} else {
    //    serial->send(str, 2+strlen(str+3));
    //}

    return true;
}


bool moveminissc(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    auto pos = (unsigned char)((int)(positions[j] * 1.411) + 127);

    char cmd[3];

    //ignore speed;
    cmd[0] = 255;              //sync byte
    cmd[1] = (unsigned char)j; //servo number byte
    cmd[2] = pos;              //position byte

    serial->send(cmd, 3);

    return true;
}


bool movepontech(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    auto pos = (unsigned char)((int)(positions[j] * 1.411) + 127);

    Bottle bot;
    char str[80];
    std::snprintf(str, 80, "BD1SV%dM%d", j + 1, pos);

    bot.addString(str);

    serial->send(bot);

    return true;
}


bool movemondotronic(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    auto pos = (unsigned char)((int)(positions[j] * 1.411) + 127);

    char cmd[3];

    //ignore speed;
    cmd[0] = 255;              //sync byte
    cmd[1] = (unsigned char)j; //servo number byte
    cmd[2] = pos;              //position byte (speed. this board controls speed for dc motors)

    serial->send(cmd, 3);

    return true;
}


bool movepololu(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    int pos = 1500 + round(positions[j] * 11.11);

    char cmd[6];

    cmd[0] = 0x80;
    cmd[1] = 0x01;

    cmd[2] = 0x04;
    cmd[3] = (unsigned char)j;

    cmd[4] = (unsigned char)(pos >> 8); //high pos byte
    cmd[5] = (unsigned char)pos;        //low pos byte

    serial->send(cmd, 6);

    return true;
}


bool movepicopic(int j, double ref, double* positions, double* speeds, ISerialDevice* serial)
{
    int pos = 1500 + round(positions[j] * 11.11);

    char cmd[5];

    cmd[0] = (int)(j / 20) + 1; //board address
    cmd[1] = (j + 1);           //servo number (1-....)

    cmd[2] = (unsigned char)(pos >> 8); //high pos byte
    cmd[3] = (unsigned char)pos;        //low pos byte

    if (FABS(speeds[j]) < 0.1) {
        cmd[4] = 255; //speed
    } else {
        auto speed = (unsigned char)((int)(speeds[j] * 1.411) + 127);

        cmd[4] = speed; //speed
    }

    serial->send(cmd, 5);

    return true;
}
