/*
 * Copyright (C) 2008 Giacomo Spigler
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/dev/SerialInterfaces.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define FABS(x) (x>=0?x:-x)

#define SSC32 1
#define MINISSC 2
#define PONTECHSV203X 3
#define MONDOTRONICSMI 4
#define POLOLUUSB16 6
#define PICOPIC 7


using namespace yarp::os;
using namespace yarp::dev;



bool movessc32(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);
bool moveminissc(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);
bool movepontech(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);
bool movemondotronic(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);
bool movepololu(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);
bool movepicopic(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);



class SerialServoBoard : public DeviceDriver, public IPositionControl {
public:
    int servoboard;

    ISerialDevice *serial;

    PolyDriver dd;

    bool (*move)(int j, double ref, double *positions, double *speeds, ISerialDevice *serial);

    double *positions;
    double *speeds;

    bool getAxes(int *ax);
    bool setPositionMode();
    bool positionMove(int j, double ref);
    bool positionMove(const double *refs);
    bool relativeMove(int j, double delta);
    bool relativeMove(const double *deltas);
    bool checkMotionDone(int j, bool *flag);
    bool checkMotionDone(bool *flag);
    bool setRefSpeed(int j, double sp);
    bool setRefSpeeds(const double *spds);
    bool setRefAcceleration(int j, double acc);
    bool setRefAccelerations(const double *accs);
    bool getRefSpeed(int j, double *ref);
    bool getRefSpeeds(double *spds);
    bool getRefAcceleration(int j, double *acc);
    bool getRefAccelerations(double *accs);
    bool stop(int j);
    bool stop();

    virtual bool open(Searchable& config) {
        if(config.check("help")==true) {
          printf("SerialServoBoard Available Options:\n");
          printf(" -board NAME, where name is one of ssc32, minissc, pontech_sv203x, mondotronic_smi, parallax, pololu_usb_16servo, picopic\n");
          printf(" -comport NAME, where name is COMx on Windows, and /dev/ttySx on Linux\n");
          printf(" -baudrate RATE, where RATE is the Board baudrate\n");
          printf("\n -help shows this help\n");

          return false;
        }


        char servoboard_[80];

        strcpy(servoboard_, config.check("board", yarp::os::Value("ssc32")).asString().c_str());

        if(strcmp(servoboard_, "ssc32")==0) {
          servoboard=SSC32;
          move=&movessc32;
        }else if(strcmp(servoboard_, "minissc")==0) {
          servoboard=MINISSC;
          move=&moveminissc;
        }else if(strcmp(servoboard_, "pontech_sv203x")==0) {
          servoboard=PONTECHSV203X;
          move=&movepontech;
        }else if(strcmp(servoboard_, "mondotronic_smi")==0) {
          servoboard=MONDOTRONICSMI;
          move=&movemondotronic;
        }else if(strcmp(servoboard_, "pololu_usb_16servo")==0) {
          servoboard=POLOLUUSB16;
          move=&movepololu;
        }else if(strcmp(servoboard_, "picopic")==0) {
          servoboard=PICOPIC;
          move=&movepicopic;
        }



        char comport[80];

        strcpy(comport, config.check("comport", yarp::os::Value("/dev/ttyS0")).asString().c_str());

        int baudrate = config.check("baudrate", yarp::os::Value(38400)).asInt();

        Property conf;
        // no arguments, use a default
        char str[160];

        sprintf(str, "(device serialport) (comport %s) (baudrate %d) (rcvenb 1) (stopbits 2) (databits 8) (paritymode none)", comport, baudrate);

        conf.fromString(str);


        dd.open(conf);
        if (!dd.isValid()) {
            printf("Failed to create and configure a device\n");
            exit(1);
        }

        if (!dd.view(serial)) {
            printf("Failed to view device through IGPUDevice interface\n");
            exit(1);
        }



        positions=(double *)malloc(sizeof(double)*32);
        speeds=(double *)malloc(sizeof(double)*32);

        return true;
    }

    virtual bool close() {
        dd.close();

        free(positions);
        free(speeds);

        return true;
    }
};



