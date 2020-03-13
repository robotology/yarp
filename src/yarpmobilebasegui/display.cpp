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
    if (scene)
    {
        delete scene;
        scene = nullptr;
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
        case Qt::Key_I: max_vel_lin += 0.05; yDebug() << "linear velocity:" << max_vel_lin << "m/s"; break;
        case Qt::Key_K: if (max_vel_lin > 0) { max_vel_lin -= 0.05; } yDebug() << "Linear velocity:" << max_vel_lin << "m/s"; break;
        case Qt::Key_O: max_vel_theta += 5; yDebug() << "angular velocity:" << max_vel_theta << "deg/s"; break;
        case Qt::Key_L: if (max_vel_theta > 0) { max_vel_theta -= 5; } yDebug() << "Angular velocity:" << max_vel_theta << "deg/s"; break;
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

    outputport.write(output_data);

    return;
}

MainWindow::MainWindow(const yarp::os::ResourceFinder& rf, QWidget *parent, double refresh_period) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(1000*refresh_period); //10 seconds

    //this->setWindowFlags(Qt::BypassWindowManagerHint); //Set window with no title bar
    //this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint); //Set window to fixed size
   // this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window
    this->setWindowFlags(Qt::WindowStaysOnTopHint); //Always on  top

    bool ret_load = true;
    ret_load &= img_background1.load(":/images/controls.png");
    if (ret_load == false)
    {
        yError("Failed loading graphics");
    }
    size_t iwidth=278;
    size_t iheight=319;

    outputport.open("/keyboardControl:o");

    scene = new QGraphicsScene;
    scene->clear();
    scene->setSceneRect(0, 0, iwidth, iheight);

    //the background
    QRect rect(0, 0, iwidth, iheight);
    QPixmap qpm = img_background1.copy(rect);
    QGraphicsPixmapItem* p1 = scene->addPixmap(qpm);
    p1->setScale(1);
    p1->setFlag(QGraphicsItem::ItemIsMovable, true);
    p1->setPos(0, 0);

    ui->graphicsView->setScene(scene);

    QTimer::singleShot(0, this, &MainWindow::updateMain);
}
