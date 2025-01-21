/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
#include "joywidget.h"

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const yarp::os::ResourceFinder& rf, QWidget *parent = 0, double refresh_period=0.01);
    ~MainWindow();

    private slots:
    void updateMain();
    void handleButton_Wp();
    void handleButton_Wr();
    void handleButton_Sp();
    void handleButton_Sr();
    void handleButton_Ap();
    void handleButton_Ar();
    void handleButton_Dp();
    void handleButton_Dr();
    void handleButton_Qp();
    void handleButton_Qr();
    void handleButton_Ep();
    void handleButton_Er();

    void handleButton_I();
    void handleButton_K();
    void handleButton_O();
    void handleButton_L();
protected:
    void keyReleaseEvent(QKeyEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

public:
    yarp::os::Network                yarp;

public:
    //parametes
    double max_vel_lin = 1; //ms/s
    double max_vel_theta = 10; //deg/s
    double cur_vel_lin_x = 0; //ms/s
    double cur_vel_lin_y = 0; //ms/s
    double cur_vel_theta = 0; //deg/s
    double lin_vel_step = 0.05;//ms/s
    double ang_vel_step = 5;  //deg/s
    std::string portname = "/yarpmobilebasegui:o";

#define s_max_lin_vel "max linear velocity: %3.3f m/s"
#define s_max_ang_vel "max angular velocity: %3.1f deg/s"
#define s_cur_lin_vel_x "cur x linear velocity: %3.3f m/s"
#define s_cur_lin_vel_y "cur y linear velocity: %3.3f m/s"
#define s_cur_ang_vel "cur angular velocity: %3.1f deg/s"

private:
    Ui_MainWindow                    *ui;
    QTimer*                          mainTimer;
    Joywidget*                       joy;
    char buff[100];

public:
    yarp::os::Port                   outputport;
    bool pressed_left = false;
    bool pressed_up = false;
    bool pressed_down = false;
    bool pressed_right = false;
    bool pressed_turn_left = false;
    bool pressed_turn_right = false;
    yarp::dev::MobileBaseVelocity output_data;
};

#endif
