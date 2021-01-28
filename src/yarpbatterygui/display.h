/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    explicit MainWindow(const yarp::os::ResourceFinder& rf, yarp::dev::IBattery* ibat, QWidget *parent = 0, double refresh_period=10.0);
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
    std::string            info;
};

#endif
