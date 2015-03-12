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

///////////GTK///////////
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <yarp/os/Time.h>

extern GtkWidget *window;

GtkWidget * dialog_message_generator(GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);
bool dialog_message (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);
bool dialog_severe_error (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect);

//*********************************************************************************
// This callback exits from the gtk_main() main loop when the main window is closed
//static void destroy_main (GtkWindow *window,	gpointer   user_data)
//{
//  gtk_widget_destroy (GTK_WIDGET(window));
//  gtk_main_quit ();
//}
