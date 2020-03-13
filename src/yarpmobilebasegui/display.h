/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/MobileBaseVelocity.h>

#include "ui_display.h"

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const yarp::os::ResourceFinder& rf, QWidget *parent = 0, double refresh_period=0.01);
    ~MainWindow();

    private slots:
    void updateMain();

protected:
    void keyReleaseEvent(QKeyEvent* e);
    void keyPressEvent(QKeyEvent* e);

public:
    yarp::os::Network                yarp;
    QGraphicsScene*                  scene;

public:
    yarp::os::Port                   outputport;
    bool pressed_left = false;
    bool pressed_up = false;
    bool pressed_down = false;
    bool pressed_right = false;
    bool pressed_turn_left = false;
    bool pressed_turn_right = false;
    double max_vel_lin = 1; //ms/s
    double max_vel_theta = 10; //deg/s
    yarp::dev::MobileBaseVelocity output_data;


private:
    Ui_MainWindow                    *ui;
    QTimer*                          mainTimer;

    QPixmap                          img_background1;
};

#endif
