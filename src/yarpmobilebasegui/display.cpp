/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifdef MSVC
    #define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "display.h"
#include <QGraphicsPixmapItem>
#include <QBitmap>
#include <QKeyEvent>


using namespace std;

MainWindow::~MainWindow()
{
    if (mainTimer)
    {
        delete mainTimer;
        mainTimer = nullptr;
    }
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
    if (joy)
    {
        delete joy;
        joy = nullptr;
    }
    outputport.close();
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_A: pressed_left = false; break;
        case Qt::Key_W: pressed_up = false; break;
        case Qt::Key_S: pressed_down = false; break;
        case Qt::Key_D: pressed_right = false; break;
        case Qt::Key_Q: pressed_turn_left = false; break;
        case Qt::Key_E: pressed_turn_right = false; break;
    }
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_A: pressed_left = true; break;
        case Qt::Key_W: pressed_up = true; break;
        case Qt::Key_S: pressed_down = true; break;
        case Qt::Key_D: pressed_right = true; break;
        case Qt::Key_Q: pressed_turn_left = true; break;
        case Qt::Key_E: pressed_turn_right = true; break;
        case Qt::Key_I: max_vel_lin += lin_vel_step;
            snprintf(buff, 100, s_max_lin_vel, max_vel_lin);
            yDebug() << buff;
            ui->label_max_lin_vel->setText(buff);
        break;
        case Qt::Key_K: if (max_vel_lin > 0) { max_vel_lin -= lin_vel_step; }
            snprintf(buff, 100, s_max_lin_vel, max_vel_lin);
            yDebug() << buff;
            ui->label_max_lin_vel->setText(buff);
        break;
        case Qt::Key_O: max_vel_theta += ang_vel_step;
            snprintf(buff, 100, s_max_ang_vel, max_vel_theta);
            yDebug() << buff;
            ui->label_max_ang_vel->setText(buff);
        break;
        case Qt::Key_L: if (max_vel_theta > 0) { max_vel_theta -= ang_vel_step; }
            snprintf(buff, 100, s_max_ang_vel, max_vel_theta);
            yDebug() << buff;
            ui->label_max_ang_vel->setText(buff);
        break;
    }
}

void MainWindow::updateMain()
{
    if (pressed_left) { output_data.vel_y = max_vel_lin; }
    else if (pressed_right) { output_data.vel_y = -max_vel_lin; }
    else { output_data.vel_y = 0; }

    if (pressed_up) { output_data.vel_x = max_vel_lin; }
    else if (pressed_down) { output_data.vel_x = -max_vel_lin; }
    else { output_data.vel_x = 0; }

    if (pressed_turn_left) { output_data.vel_theta = max_vel_theta; }
    else if (pressed_turn_right) { output_data.vel_theta = -max_vel_theta; }
    else { output_data.vel_theta = 0; }

    double vel_lin_percent = 0;
    double vel_dir = 0;
    this->joy->getStatus(vel_lin_percent, vel_dir);
    if (vel_lin_percent > 0)
    {
        output_data.vel_x = -vel_lin_percent * max_vel_lin * cos(vel_dir);
        output_data.vel_y = -vel_lin_percent * max_vel_lin * sin(vel_dir);
    }


    cur_vel_lin_x = output_data.vel_x;
    cur_vel_lin_y = output_data.vel_y;

    cur_vel_theta = output_data.vel_theta;

    snprintf(buff, 100, s_cur_lin_vel_x, cur_vel_lin_x);
    ui->label_cur_lin_vel_x->setText(buff);
    snprintf(buff, 100, s_cur_lin_vel_y, cur_vel_lin_y);
    ui->label_cur_lin_vel_y->setText(buff);

    snprintf(buff, 100, s_cur_ang_vel, cur_vel_theta);
    ui->label_cur_ang_vel->setText(buff);

    outputport.write(output_data);

    return;
}

MainWindow::MainWindow(const yarp::os::ResourceFinder& rf, QWidget *parent, double refresh_period) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("yarpMobilebaseGUI");

    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(1000*refresh_period); //10 seconds

    //this->setWindowFlags(Qt::BypassWindowManagerHint); //Set window with no title bar
    //this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint); //Set window to fixed size
   // this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window
    this->setWindowFlags(Qt::WindowStaysOnTopHint); //Always on  top


    yInfo() << "--port <name>         the full name of the output port";
    yInfo() << "--step_lin_vel        the increase step of linear velocity (m/s)";
    yInfo() << "--step_ang_vel        the increase step of angular velocity (deg/s)";
    yInfo() << "--default_lin_vel     the default value for max linear velocity (m/s)";
    yInfo() << "--default_ang_vel     the default value for max angular velocity (deg/s)";

    if (rf.check("port"))
        { portname = rf.find("port").asString(); }
    if (rf.check("step_lin_vel"))
        { lin_vel_step = rf.find("step_lin_vel").asFloat64(); }
    if (rf.check("step_ang_vel"))
        { ang_vel_step = rf.find("step_ang_vel").asFloat64(); }
    if (rf.check("default_lin_vel"))
        { max_vel_lin = rf.find("default_lin_vel").asFloat64(); }
    if (rf.check("default_ang_vel"))
        { max_vel_theta = rf.find("default_ang_vel").asFloat64(); }

    outputport.open(portname);

    connect(ui->pushButton_W, SIGNAL(pressed()), this, SLOT(handleButton_Wp()));
    connect(ui->pushButton_W, SIGNAL(released()), this, SLOT(handleButton_Wr()));
    connect(ui->pushButton_S, SIGNAL(pressed()), this, SLOT(handleButton_Sp()));
    connect(ui->pushButton_S, SIGNAL(released()), this, SLOT(handleButton_Sr()));
    connect(ui->pushButton_A, SIGNAL(pressed()), this, SLOT(handleButton_Ap()));
    connect(ui->pushButton_A, SIGNAL(released()), this, SLOT(handleButton_Ar()));
    connect(ui->pushButton_D, SIGNAL(pressed()), this, SLOT(handleButton_Dp()));
    connect(ui->pushButton_D, SIGNAL(released()), this, SLOT(handleButton_Dr()));
    connect(ui->pushButton_Q, SIGNAL(pressed()), this, SLOT(handleButton_Qp()));
    connect(ui->pushButton_Q, SIGNAL(released()), this, SLOT(handleButton_Qr()));
    connect(ui->pushButton_E, SIGNAL(pressed()), this, SLOT(handleButton_Ep()));
    connect(ui->pushButton_E, SIGNAL(released()), this, SLOT(handleButton_Er()));

    connect(ui->pushButton_I, SIGNAL(clicked()), this, SLOT(handleButton_I()));
    connect(ui->pushButton_K, SIGNAL(clicked()), this, SLOT(handleButton_K()));
    connect(ui->pushButton_O, SIGNAL(clicked()), this, SLOT(handleButton_O()));
    connect(ui->pushButton_L, SIGNAL(clicked()), this, SLOT(handleButton_L()));

    snprintf(buff, 100, s_max_lin_vel, max_vel_lin);
    ui->label_max_lin_vel->setText(buff);

    snprintf(buff, 100, s_cur_lin_vel_x, cur_vel_lin_x);
    ui->label_cur_lin_vel_x->setText(buff);

    snprintf(buff, 100, s_cur_lin_vel_y, cur_vel_lin_y);
    ui->label_cur_lin_vel_y->setText(buff);

    snprintf(buff, 100, s_max_ang_vel, max_vel_theta);
    ui->label_max_ang_vel->setText(buff);

    snprintf(buff, 100, s_cur_ang_vel, cur_vel_theta);
    ui->label_cur_ang_vel->setText(buff);

    joy = new Joywidget(ui->frame_2, "joy1",135);
    joy->move(QPoint(102,35));
    joy->show();

    QTimer::singleShot(0, this, &MainWindow::updateMain);
}


void MainWindow::handleButton_Wp()
{
    pressed_up = true;
}
void MainWindow::handleButton_Wr()
{
    pressed_up = false;
}

void MainWindow::handleButton_Sp()
{
    pressed_down = true;
}
void MainWindow::handleButton_Ap()
{
    pressed_left = true;
}
void MainWindow::handleButton_Dp()
{
    pressed_right = true;
}
void MainWindow::handleButton_Qp()
{
    pressed_turn_left = true;
}
void MainWindow::handleButton_Ep()
{
    pressed_turn_right = true;
}

void MainWindow::handleButton_Sr()
{
    pressed_down = false;
}
void MainWindow::handleButton_Ar()
{
    pressed_left = false;
}
void MainWindow::handleButton_Dr()
{
    pressed_right = false;
}
void MainWindow::handleButton_Qr()
{
    pressed_turn_left = false;
}
void MainWindow::handleButton_Er()
{
    pressed_turn_right = false;
}


void MainWindow::handleButton_I()
{
    max_vel_lin += lin_vel_step;
    if (max_vel_lin < 0) max_vel_lin = 0;
    snprintf(buff,100, s_max_lin_vel, max_vel_lin);
    ui->label_max_lin_vel->setText(buff);
}
void MainWindow::handleButton_K()
{
    max_vel_lin -= lin_vel_step;
    if (max_vel_lin < 0) max_vel_lin = 0;
    snprintf(buff, 100, s_max_lin_vel, max_vel_lin);
    ui->label_max_lin_vel->setText(buff);
}
void MainWindow::handleButton_O()
{
    max_vel_theta += ang_vel_step;
    if (max_vel_theta < 0) max_vel_theta = 0;
    snprintf(buff, 100, s_max_ang_vel, max_vel_theta);
    ui->label_max_ang_vel->setText(buff);
}
void MainWindow::handleButton_L()
{
    max_vel_theta -= ang_vel_step;
    if (max_vel_theta < 0) max_vel_theta = 0;
    snprintf(buff, 100, s_max_ang_vel, max_vel_theta);
    ui->label_max_ang_vel->setText(buff);
}
