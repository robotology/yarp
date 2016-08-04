/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo
* Date: June 2015
* email:   marco.randazzo@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QMainWindow>
#include <QTimer>

#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include "ui_display.h"

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(yarp::os::ResourceFinder rf, yarp::dev::IBattery* ibat, QWidget *parent = 0);
    ~MainWindow();

    private slots:

    void updateMain();

public:
    yarp::os::Network                yarp;
    yarp::dev::IBattery*             ibat;
    yarp::dev::PolyDriver*           drv;
    QGraphicsScene*                  scene;

private:
    Ui_MainWindow                    *ui;
    QTimer*                          mainTimer;

    QPixmap                          img_background1;
    QPixmap                          img_background2;
    QPixmap                          img_blocks;
    QPixmap                          img_charge;
    QPixmap                          img_numbers;
    bool                             connected;
    bool                             enable_ask_info;

    //data read from the battery
    double                           voltage;
    double                           current;
    double                           charge;
    yarp::os::ConstString            info;
};

#endif
