/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef YARPMOTORGUI_H
#define YARPMOTORGUI_H
#include <QColor>

#include <yarp/os/ResourceFinder.h>

const int MAX_NUMBER_OF_PARTS = 16;
const int NUMBER_OF_STORED = 30;

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
const QColor hwFaultColor        = QColor(255,50,50);
const QColor calibratingColor    = QColor(220,220,220);

enum {
  POS_SEQUENCE,
  TIM_SEQUENCE,
  ADDITIONAL_COLUMNS
};


#endif // YARPMOTORGUI_H
