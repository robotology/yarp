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


#ifndef GUICONTROL_H
#define GUICONTROL_H

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "partMover.h"

namespace guiControl
{
    static GtkWidget *pos_winPid = NULL;
    static GtkWidget *label_title = NULL;
    static GtkWidget *radiobutton_mode_idl = NULL;
    static GtkWidget *radiobutton_mode_pos = NULL;
    static GtkWidget *radiobutton_mode_pos_direct = NULL;
    static GtkWidget *radiobutton_mode_vel = NULL;
    static GtkWidget *radiobutton_mode_mixed = NULL;
    static GtkWidget *radiobutton_mode_trq = NULL;
    static GtkWidget *radiobutton_mode_imp_pos = NULL;
    static GtkWidget *radiobutton_mode_imp_vel = NULL;
    static GtkWidget *radiobutton_mode_open = NULL;

    static int * joint = NULL;
    static IControlMode *icntrl = NULL;
    static IAmplifierControl *iamp = NULL;
    static IPidControl *ipid = NULL;

    void guiControl(void *button, void* data);
    void destroy_main (GtkWindow *window,    gpointer   user_data);
    void destroy_win (GtkButton *button, GtkWindow *window);
    void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label);
    void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label);
    void update_menu(int control_mode);

    static void radio_click_idl (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_pos (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_pos_direct (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_mode_mixed (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_vel (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_trq (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_imp_pos (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_imp_vel (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_open(GtkWidget* radio , gtkClassData* currentClassData);    
};

#endif
