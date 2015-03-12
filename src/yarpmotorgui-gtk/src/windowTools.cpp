/*
 * Copyright (C)2012  Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
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

#include "include/windowTools.h"

GtkWidget *message            = NULL;
GtkWidget *message_icon       = NULL;
GtkWidget *message_hbox       = NULL;
GtkWidget *message_right_vbox = NULL;
GtkWidget *message_label1     = NULL;
GtkWidget *message_label2     = NULL;

//Shows a message dialog for displaying infos/errors
GtkWidget * dialog_message_generator(GtkMessageType gtk_message_type, const char *text1, const char *text2)
{
    GtkWidget *message;

    //message=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,gtk_message_type,GTK_BUTTONS_CLOSE, text1);


    if (gtk_message_type!=GTK_MESSAGE_QUESTION)
    {
        message = gtk_dialog_new_with_buttons ("Interactive Dialog",
            GTK_WINDOW (window),
            GTK_DIALOG_MODAL,
            GTK_STOCK_OK,
            GTK_RESPONSE_OK,
            NULL);
    }
    else
    {
        message = gtk_dialog_new_with_buttons ("Interactive Dialog",
            GTK_WINDOW (window),
            GTK_DIALOG_MODAL,
            GTK_STOCK_YES,
            GTK_RESPONSE_YES,
            GTK_STOCK_NO,
            GTK_RESPONSE_NO,
            NULL);
    }

    /*
    message = gtk_dialog_new ("",
    GTK_WINDOW (window),
    GTK_DIALOG_MODAL,
    NULL);
    */

    gtk_window_set_resizable(GTK_WINDOW(message),false);

    message_hbox = gtk_hbox_new (FALSE, 8);
    gtk_container_set_border_width (GTK_CONTAINER (message_hbox), 8);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (message)->vbox), message_hbox, FALSE, FALSE, 0);

    if      (gtk_message_type==GTK_MESSAGE_QUESTION)
    {
        gtk_window_set_title    (GTK_WINDOW(message),"Question");
        message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
    }
    else if (gtk_message_type==GTK_MESSAGE_ERROR)
    {
        gtk_window_set_title    (GTK_WINDOW(message),"Error");
        message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
    }
    else if (gtk_message_type==GTK_MESSAGE_INFO)
    {
        gtk_window_set_title    (GTK_WINDOW(message),"Information");
        message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    }
    else
    {
        gtk_window_set_title    (GTK_WINDOW(message),"Information");
        message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    }

    gtk_box_pack_start (GTK_BOX (message_hbox), message_icon, FALSE, FALSE, 0);

    message_right_vbox = gtk_vbox_new (FALSE, 8);
    gtk_container_set_border_width (GTK_CONTAINER (message_right_vbox), 8);
    gtk_box_pack_start (GTK_BOX (message_hbox), message_right_vbox, FALSE, FALSE, 0);

    message_label1 = gtk_label_new (text1);

    gtk_label_set_justify   (GTK_LABEL(message_label1),  GTK_JUSTIFY_LEFT);
    gtk_box_pack_start (GTK_BOX (message_right_vbox), message_label1, FALSE, FALSE, 0);

    if (text2)
    {
        message_label2 = gtk_label_new (text2);

        gtk_label_set_justify   (GTK_LABEL(message_label2),  GTK_JUSTIFY_LEFT);
        gtk_box_pack_start (GTK_BOX (message_right_vbox), message_label2, FALSE, FALSE, 0);
    }

    gtk_widget_show_all (message_hbox);

    //GTK_BUTTONS_YES_NO
    return message;
}

bool dialog_question (const char* text)
{
    bool ret=false;

    message=dialog_message_generator(GTK_MESSAGE_QUESTION, text, NULL);
    gtk_window_set_modal(GTK_WINDOW(message),true);

    gint response = gtk_dialog_run (GTK_DIALOG (message));
    if (response == GTK_RESPONSE_YES)
    {
        ret=true;
    }
    else
    {
        ret=false;
    }
    gtk_widget_destroy (message);
    return ret;
}
