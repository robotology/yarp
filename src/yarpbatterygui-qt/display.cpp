/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo
* Date: June 2015
* email:   marco.randazzo@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <math.h>
#include "display.h"
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <QGraphicsPixmapItem>
#include <QBitmap>

#ifndef M_PI
#define M_PI 3.1415927
#endif

using namespace std;

MainWindow::~MainWindow()
{
    if (mainTimer)
    {
        delete mainTimer;
        mainTimer = 0;
    }
    if (ui)
    {
        delete ui;
        ui = 0;
    }
    if (scene)
    {
        delete scene;
        scene = 0;
    }
}

void MainWindow::updateMain()
{
    static double voltage = 0;
    static double current = 0;
    static double charge = 0;

    if (ibat)
    {
        //yDebug("received battery info");
        bool ret = true;
        ret &= ibat->getBatteryVoltage(voltage);
        ret &= ibat->getBatteryCurrent(current);
        ret &= ibat->getBatteryCharge(charge);
        connected = ret;
    }
    else
    {
        connected = false;
        yError("TIMEOUT: unable to receive data from battery manager ");
    }

    scene->clear();
    scene->setSceneRect(0, 0, 200, 180);

    //For debug purpose only
    //connected = true;
    //charge = 100; 
    //voltage = 40.1;
    //current = -10.3;

    //the background
    if (connected && charge > 12)
    {
        QRect rect(0, 0, 200, 180);
        QPixmap qpm = img_background1.copy(rect);
        QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
        p1->setScale(1);
        p1->setFlag(QGraphicsItem::ItemIsMovable, true);
        p1->setPos(0, 0);
    }
    else
    {
        QRect rect(0, 0, 200, 180);
        QPixmap qpm = img_background2.copy(rect);
        QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
        p1->setScale(1);
        p1->setFlag(QGraphicsItem::ItemIsMovable, true);
        p1->setPos(0, 0);
    }

    //the charge text
    {
        if (current<-0.3)
        {
            QRect rect(0, 8, 48, 8);
            QPixmap qpm = img_charge.copy(rect);
            QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
            p1->setScale(1);
            p1->setFlag(QGraphicsItem::ItemIsMovable, true);
            p1->setPos(56, 77);
        }
        else
        {
            QRect rect(0, 0, 48, 8);
            QPixmap qpm = img_charge.copy(rect);
            QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
            p1->setScale(1);
            p1->setFlag(QGraphicsItem::ItemIsMovable, true);
            p1->setPos(56, 77);
        }
    }

    //the charge indicator
    {
        QRect rect0(0, 0, 14, 7);
        QRect rect1(0, 15, 14, 7);
        QRect rect2(0, 30, 14, 7);
        QPixmap  qpm0 = img_blocks.copy(rect0);
        QPixmap  qpm1 = img_blocks.copy(rect1);
        QPixmap  qpm2 = img_blocks.copy(rect2);
        QPixmap* qpp = &qpm2;
        int n_blocks = int(charge * 11 / 100.0);
        for (int i = 0; i < n_blocks; i++)
        {
            if (current < -0.3) qpp = &qpm0;   //draw charging arrows
            else                qpp = &qpm1;   //draw standard boxes

            int xpos = 166;
            int ypos = 135 - i * 6;

            QGraphicsPixmapItem *p1 = scene->addPixmap(*qpp);
            p1->setScale(1);
            p1->setFlag(QGraphicsItem::ItemIsMovable, true);
            p1->setPos(xpos, ypos);
        }
    }

    //the voltage
    {
        char buff[10];
        sprintf(buff, "%4.1f", voltage);
        int len = strlen(buff);
        int point_off = 0;
        for (int i = 0; i < len; i++)
        {
            if (buff[i] == '.') point_off = 17;
            if (buff[i] >= '0' && buff[i] <= '9')
            {
                QRect rect((buff[i] - '0') * 29, 0, 29, 52);
                QPixmap  qpm = img_numbers.copy(rect);
                QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
                p1->setScale(1);
                p1->setFlag(QGraphicsItem::ItemIsMovable, true);
                p1->setPos(19 + i * 29 - point_off, 21);
            }
        }
    }

    //the current
    {
        char buff[10];
        sprintf(buff, "%4.1f", fabs(current));
        int len = strlen(buff);
        int point_off = 0;
        for (int i = 0; i < len; i++)
        {
            if (buff[i] == '.') point_off = 17;
            if (buff[i] >= '0' && buff[i] <= '9')
            {
                QRect rect((buff[i] - '0') * 29, 0, 29, 52);
                QPixmap  qpm = img_numbers.copy(rect);
                QGraphicsPixmapItem *p1 = scene->addPixmap(qpm);
                p1->setScale(1);
                p1->setFlag(QGraphicsItem::ItemIsMovable, true);
                p1->setPos(19 + i * 29 - point_off, 88);
            }
        }
    }
    ui->graphicsView->setScene(scene);

    return;
}

MainWindow::MainWindow(yarp::os::ResourceFinder rf, yarp::dev::IBattery* p_ibat, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ibat = p_ibat;
    connected = false;

    ui->setupUi(this);

    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(1000*10); //10 seconds
    
    //this->setWindowFlags(Qt::BypassWindowManagerHint); //Set window with no title bar
    //this->setWindowFlags(Qt::CustomizeWindowHint); //Set window with no title bar
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint); //Set window to fixed size
   // this->setWindowFlags(Qt::FramelessWindowHint); //Set a frameless window
    this->setWindowFlags(Qt::WindowStaysOnTopHint); //Always on  top

    bool ret_load = true;
    ret_load &= img_background1.load(":/images/background.bmp");
    ret_load &= img_background2.load(":/images/background2.bmp");
    ret_load &= img_blocks.load(":/images/batt_blocks.bmp");
    ret_load &= img_charge.load(":/images/charge.bmp");
    ret_load &= img_numbers.load(":/images/numbers.bmp");
    if (ret_load == false)
    {
        yError("Failed loading graphics");
    }

    img_blocks.setMask(img_blocks.createMaskFromColor(QColor(255,0,255)));
    img_charge.setMask(img_charge.createMaskFromColor(QColor(255, 0, 255)));
    img_numbers.setMask(img_numbers.createMaskFromColor(QColor(255, 0, 255)));

    scene = new QGraphicsScene;
    updateMain();
}
