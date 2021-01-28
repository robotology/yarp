/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
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

#ifndef YARPMOTORGUI_H
#define YARPMOTORGUI_H
#include <QColor>

#include <yarp/os/ResourceFinder.h>

const QColor idleColor           = QColor( 249,236,141);
const QColor positionColor       = QColor( 149,221,186);
const QColor positionDirectColor = QColor( 119,206,111);
const QColor mixedColor          = QColor( 150,(221+190)/2,(186+255)/2);
const QColor velocityColor       = QColor( 150,190,255);
const QColor torqueColor         = QColor( 219,166,171);
const QColor pwmColor            = QColor( 250,250,250);
const QColor currentColor        = QColor(250, 200, 120);
const QColor errorColor          = QColor(255,0,0);
const QColor disconnectColor     = QColor(190,190,190);
const QColor hwFaultColor        = QColor(255,50,50);
const QColor calibratingColor    = QColor(220,220,220);

enum {
  POS_SEQUENCE,
  TIM_SEQUENCE,
  ADDITIONAL_COLUMNS
};


#endif // YARPMOTORGUI_H
