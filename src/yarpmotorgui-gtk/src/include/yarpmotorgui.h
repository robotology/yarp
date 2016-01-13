/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori <francesco.nori@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#ifndef YARP_YARPMOTORGUI_GTK_YARPMOTORGUI_H
#define YARP_YARPMOTORGUI_GTK_YARPMOTORGUI_H


#include <yarp/os/ResourceFinder.h>
///////////GTK///////////
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "gtkMessages.h"

const int    MAX_NUMBER_ACTIVATED = 16;
const double ARM_VELOCITY[]={10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
const int    MAX_NUMBER_OF_JOINTS = 16;
const int    NUMBER_OF_STORED = 30;

extern GtkWidget *robotNameBox;
extern int PART;
extern int NUMBER_OF_AVAILABLE_PARTS;
extern int NUMBER_OF_ACTIVATED_PARTS;
extern int NUMBER_OF_ACTIVATED_CARTESIAN;
extern char *partsName[MAX_NUMBER_ACTIVATED];
extern int        *ENA[MAX_NUMBER_ACTIVATED];

enum {
  POS_SEQUENCE,
  TIM_SEQUENCE,
  ADDITIONAL_COLUMNS
};

#endif // YARP_YARPMOTORGUI_GTK_YARPMOTORGUI_H
