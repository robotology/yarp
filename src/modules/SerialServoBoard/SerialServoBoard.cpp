/*
 * Copyright (C) 2008 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/dev/SerialInterfaces.h>

#include <stdio.h>
#include <stdlib.h>

#include "SerialServoBoard.h"


//TODO: check limits of operation (range of angles)?


using namespace yarp::os;
using namespace yarp::dev;


bool SerialServoBoard::getAxes(int *ax) {
    if(servoboard==SSC32) {
        *ax = 32;
    }else if(servoboard==MINISSC) {
        *ax = 8;
    }else if(servoboard==PONTECHSV203X) {
        *ax = 8;
    }else if(servoboard==MONDOTRONICSMI) {
        *ax = 2;
    }else if(servoboard==POLOLUUSB16) {
        *ax = 16;
    }else if(servoboard==PICOPIC) {
        *ax = 20;
    }

    return true;
}


bool SerialServoBoard::setPositionMode() {
    return true;
}


bool SerialServoBoard::positionMove(int j, double ref) {
    positions[j]=ref;

    return move(j, ref, positions, speeds, serial);
}


bool SerialServoBoard::positionMove(const double *refs) {
    for(int k=0; k<32; k++) {
        this->positionMove(k, refs[k]);
    }

    return true;
}


bool SerialServoBoard::relativeMove(int j, double delta) {
    this->positionMove(j, positions[j]+delta);

    return true;
}


bool SerialServoBoard::relativeMove(const double *deltas) {
    for(int k=0; k<32; k++) {
        this->positionMove(k, positions[k]+deltas[k]);
    }

    return true;
}


bool SerialServoBoard::checkMotionDone(int j, bool *flag) {
    //TODO: Q?

    return true;
}


bool SerialServoBoard::checkMotionDone(bool *flag) {
    //TODO: Q?

    return true;
}


bool SerialServoBoard::setRefSpeed(int j, double sp) {
    speeds[j]=sp;

    return true;
}


bool SerialServoBoard::setRefSpeeds(const double *spds) {
    for(int k=0; k<32; k++) {
        setRefSpeed(k, spds[k]);
    }

    return true;
}


bool SerialServoBoard::setRefAcceleration(int j, double acc) {
    return true;
}


bool SerialServoBoard::setRefAccelerations(const double *accs) {
    return true;
}


bool SerialServoBoard::getRefSpeed(int j, double *ref) {
    *ref=speeds[j];

    return true;
}


bool SerialServoBoard::getRefSpeeds(double *spds) {
    for(int k=0; k<32; k++) {
        spds[k]=speeds[k];
    }

    return true;
}


bool SerialServoBoard::getRefAcceleration(int j, double *acc) {
    return true;
}


bool SerialServoBoard::getRefAccelerations(double *accs) {
    return true;
}


bool SerialServoBoard::stop(int j) {
    return true;
}


bool SerialServoBoard::stop() {
    return true;
}





bool movessc32(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    int pos=1500+round(positions[j]*11.11);

    Bottle bot;
    char str[80];
    if(FABS(speeds[j])<0.1) {
        sprintf(str, "#%dP%d\r", j, pos);
    } else {
        int speed=round(speeds[j]*11.11);

        sprintf(str, "#%dP%dS%d\r", j, pos, speed);
    }
    //if(j==0) {
        bot.addString(str);
        serial->send(bot);
    //} else {
    //    serial->send(str, 2+strlen(str+3));
    //}

    return true;
}


bool moveminissc(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    unsigned char pos=(unsigned char)((int)(positions[j]*1.411) + 127);

    char cmd[3];

    //ignore speed;
    cmd[0]=255; //sync byte
    cmd[1]=(unsigned char)j; //servo number byte
    cmd[2]=pos; //position byte

    serial->send(cmd, 3);

    return true;
}


bool movepontech(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    unsigned char pos=(unsigned char)((int)(positions[j]*1.411) + 127);

    Bottle bot;
    char str[80];
    sprintf(str, "BD1SV%dM%d", j+1, pos);

    bot.addString(str);

    serial->send(bot);

    return true;
}


bool movemondotronic(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    unsigned char pos=(unsigned char)((int)(positions[j]*1.411) + 127);

    char cmd[3];

    //ignore speed;
    cmd[0]=255; //sync byte
    cmd[1]=(unsigned char)j; //servo number byte
    cmd[2]=pos; //position byte (speed. this board controls speed for dc motors)

    serial->send(cmd, 3);

    return true;
}


bool movepololu(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    int pos=1500+round(positions[j]*11.11);

    char cmd[6];

    cmd[0]=0x80;
    cmd[1]=0x01;

    cmd[2]=0x04;
    cmd[3]=(unsigned char)j;

    cmd[4]=(unsigned char)(pos>>8); //high pos byte
    cmd[5]=(unsigned char)pos; //low pos byte

    serial->send(cmd, 6);

    return true;
}


bool movepicopic(int j, double ref, double *positions, double *speeds, ISerialDevice *serial) {
    int pos=1500+round(positions[j]*11.11);

    char cmd[5];

    cmd[0]=(int)(j/20)+1; //board address
    cmd[1]=(j+1); //servo number (1-....)

    cmd[2]=(unsigned char)(pos>>8); //high pos byte
    cmd[3]=(unsigned char)pos; //low pos byte

    if(FABS(speeds[j])<0.1) {
        cmd[4]=255; //speed
    } else {
        unsigned char speed=(unsigned char)((int)(speeds[j]*1.411) + 127);

        cmd[4]=speed; //speed
    }

    serial->send(cmd, 5);

    return true;
}



