/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori <francesco.nori@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#ifndef YARP_YARPMOTORGUI_GTK_GUICONTROL_H
#define YARP_YARPMOTORGUI_GTK_GUICONTROL_H

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

    static GtkWidget *label_title2 = NULL;
    static GtkWidget *radiobutton_interaction_stiff = NULL;
    static GtkWidget *radiobutton_interaction_compl = NULL;

    static int * joint = NULL;
    static IControlMode2 *icntrl2 = NULL;
    static IInteractionMode *iinteract = NULL;
    static IAmplifierControl *iamp = NULL;
    static IPidControl *ipid = NULL;

    void guiControl(void *button, void* data);
    void destroy_main (GtkWindow *window,    gpointer   user_data);
    void destroy_win (GtkButton *button, GtkWindow *window);
    void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label);
    void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label);
    void update_menu(int control_mode, int interaction_mode);

    static void radio_click_idl (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_pos (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_pos_direct (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_mode_mixed (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_vel (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_trq (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_imp_pos (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_imp_vel (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_open(GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_stiff (GtkWidget* radio , gtkClassData* currentClassData);
    static void radio_click_compl (GtkWidget* radio , gtkClassData* currentClassData);
    static void on_key_press(GtkWidget* radio , gtkClassData* currentClassData);
}

#endif // YARP_YARPMOTORGUI_GTK_GUICONTROL_H
