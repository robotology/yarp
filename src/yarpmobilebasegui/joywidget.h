/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef JOYWIDGET_H
#define JOYWIDGET_H

#include <QMainWindow>
#include <QTimer>

#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/MobileBaseVelocity.h>

#include  <qevent.h>
#include <qwidget.h>

class Joywidget : public QWidget
{
    Q_OBJECT
public:
    Joywidget(QWidget* parent = 0, const char* name = "", int size = 100);
    ~Joywidget();

public slots:

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    //void resizeEvent(QResizeEvent* event) override;

private:
    bool   pressed;
    QPoint mousePoint;
    void   drawCursor();
    double cursorSize;
    int    joysize;

public:
    void getStatus(double& vel_lin_percent, double& vel_dir);
};

#endif
