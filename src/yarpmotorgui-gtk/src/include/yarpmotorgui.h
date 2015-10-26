// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
