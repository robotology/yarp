/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2008 Giacomo Spigler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/dev/ISerialDevice.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

    bool getAxes(int *ax) override;
    bool positionMove(int j, double ref) override;
    bool positionMove(const double *refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const double *deltas) override;
    bool checkMotionDone(int j, bool *flag) override;
    bool checkMotionDone(bool *flag) override;
    bool setRefSpeed(int j, double sp) override;
    bool setRefSpeeds(const double *spds) override;
    bool setRefAcceleration(int j, double acc) override;
    bool setRefAccelerations(const double *accs) override;
    bool getRefSpeed(int j, double *ref) override;
    bool getRefSpeeds(double *spds) override;
    bool getRefAcceleration(int j, double *acc) override;
    bool getRefAccelerations(double *accs) override;
    bool stop(int j) override;
    bool stop() override;
    bool positionMove(const int n_joint, const int *joints, const double *refs) override;
    bool relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override;
    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override;
    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override;
    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    bool stop(const int n_joint, const int *joints) override;

    bool open(Searchable& config) override {
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
        } else if(strcmp(servoboard_, "minissc")==0) {
          servoboard=MINISSC;
          move=&moveminissc;
        } else if(strcmp(servoboard_, "pontech_sv203x")==0) {
          servoboard=PONTECHSV203X;
          move=&movepontech;
        } else if(strcmp(servoboard_, "mondotronic_smi")==0) {
          servoboard=MONDOTRONICSMI;
          move=&movemondotronic;
        } else if(strcmp(servoboard_, "pololu_usb_16servo")==0) {
          servoboard=POLOLUUSB16;
          move=&movepololu;
        } else if(strcmp(servoboard_, "picopic")==0) {
          servoboard=PICOPIC;
          move=&movepicopic;
        }



        char comport[80];

        strcpy(comport, config.check("comport", yarp::os::Value("/dev/ttyS0")).asString().c_str());

        int baudrate = config.check("baudrate", yarp::os::Value(38400)).asInt32();

        Property conf;
        // no arguments, use a default
        char str[160];

        sprintf(str, "(device serialport) (comport %s) (baudrate %d) (rcvenb 1) (stopbits 2) (databits 8) (paritymode none)", comport, baudrate);

        conf.fromString(str);


        dd.open(conf);
        if (!dd.isValid()) {
            printf("Failed to create and configure a device\n");
            std::exit(1);
        }

        if (!dd.view(serial)) {
            printf("Failed to view device through IGPUDevice interface\n");
            std::exit(1);
        }



        positions=(double *)malloc(sizeof(double)*32);
        speeds=(double *)malloc(sizeof(double)*32);

        return true;
    }

    bool close() override {
        dd.close();

        free(positions);
        free(speeds);

        return true;
    }
};
