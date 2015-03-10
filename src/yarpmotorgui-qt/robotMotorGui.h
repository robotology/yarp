#ifndef ROBOTMOTORGUI_H
#define ROBOTMOTORGUI_H
#include <QColor>

/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori
 * email:  francesco.nori@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


#include <yarp/os/ResourceFinder.h>

const int   MAX_NUMBER_ACTIVATED = 8;
const double ARM_VELOCITY[]={10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0,	10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
const int   MAX_NUMBER_OF_JOINTS = 16;
const int   	NUMBER_OF_STORED = 30;

//extern GtkWidget *robotNameBox;
extern int PART;
extern int NUMBER_OF_AVAILABLE_PARTS;
extern int NUMBER_OF_ACTIVATED_PARTS;
extern int NUMBER_OF_ACTIVATED_CARTESIAN;
//extern char *partsName[MAX_NUMBER_ACTIVATED];
//extern int        *ENA[MAX_NUMBER_ACTIVATED];


const QColor idleColor           = QColor( 249,236,141);
const QColor positionColor       = QColor( 149,221,186);
const QColor positionDirectColor = QColor( 119,206,111);
const QColor mixedColor          = QColor( 150,(221+190)/2,(186+255)/2);
const QColor velocityColor       = QColor( 150,190,255);
const QColor torqueColor         = QColor( 219,166,171);
const QColor openLoopColor       = QColor( 250,250,250);
const QColor errorColor          = QColor(255,0,0);
const QColor disconnectColor     = QColor(190,190,190);
const QColor hwFaultColor        = QColor(255,0,0);
const QColor calibratingColor    = QColor(220,220,220);

enum {
  POS_SEQUENCE,
  TIM_SEQUENCE,
  ADDITIONAL_COLUMNS
};


#endif // ROBOTMOTORGUI_H
