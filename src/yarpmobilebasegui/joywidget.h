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
