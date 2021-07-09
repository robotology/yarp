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
#include <qpainter.h>

#include "joywidget.h"

Joywidget::Joywidget(QWidget* parent, const char* name, int size)
{
    joysize=size;
    this->setParent(parent);
    this->setObjectName(name);
    this->resize(QSize(size+5, size+5));
    pressed = false;
    mousePoint.setX(size/2);
    mousePoint.setY(size/2);
    this->update();
    cursorSize=10;
}

Joywidget::~Joywidget()
{
}

void Joywidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mousePoint = event->pos();
        pressed = true;
    }
}

void Joywidget::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) && pressed)
    {
        QPoint currentPos = event->pos();
        double x = currentPos.x() - (joysize /2);
        double y = currentPos.y() - (joysize /2);
        double centerDistance = sqrt((x*x)+(y*y));
        double centerDirection = atan2(x,y);
        if (centerDistance < (joysize/2 -cursorSize))
        {
            mousePoint = currentPos;
        }
        else
        {
            double newCenterDistance = (joysize/2 -cursorSize);
            double newX = newCenterDistance * sin(centerDirection)+ joysize/2;
            double newY = newCenterDistance * cos(centerDirection)+ joysize/2;
            mousePoint = QPoint(newX, newY);
        }
        drawCursor();
    }
}

void Joywidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && pressed)
    {
        mousePoint.setX(joysize/2);
        mousePoint.setY(joysize/2);
        pressed = false;
        drawCursor();
    }
}

void Joywidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(QColor("#000000"));
    painter.setBrush(QBrush("#FFFFFF"));
    painter.drawRect(QRect(QPoint(0, 0), QSize(joysize, joysize)));

    painter.setPen(QColor("#000000"));
    painter.setBrush(QBrush("#909090"));
    painter.drawEllipse(QPoint(joysize / 2, joysize / 2), joysize / 2, joysize / 2);

    painter.setPen(QColor("#000000"));
    painter.setBrush(QBrush("#000000"));
    painter.drawEllipse(mousePoint, int(cursorSize), int(cursorSize));
}

void Joywidget::drawCursor()
{
    this->update();
}

void Joywidget::getStatus(double& vel_lin_percent, double& vel_dir)
{
    double x = mousePoint.x() - joysize/2;
    double y = mousePoint.y() - joysize/2;
    double centerDistance = sqrt((x * x) + (y * y));
    vel_lin_percent = (centerDistance) / (double(joysize) / 2);
    vel_dir = atan2(x, y);
}
