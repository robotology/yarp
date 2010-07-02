#include "include/gtkMessages.h"

GtkWidget *window;

static void destroy_main (GtkWindow *window,	gpointer   user_data)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  gtk_main_quit ();
}

GtkWidget * dialog_message_generator(GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect=true)
{
  GtkWidget *message;
		
  //message=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,gtk_message_type,GTK_BUTTONS_CLOSE, text1);
		
		
  if		(gtk_message_type!=GTK_MESSAGE_QUESTION)
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
  GtkWidget *message_hbox 	 = NULL; 		
  GtkWidget *message_icon 	 = NULL;
  GtkWidget *message_right_vbox = NULL;
  GtkWidget *message_label1	 = NULL; 
  GtkWidget *message_label2	 = NULL; 

  gtk_window_set_resizable(GTK_WINDOW(message),false);
		
  message_hbox = gtk_hbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (message_hbox), 8);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (message)->vbox), message_hbox, FALSE, FALSE, 0);
		
  if		(gtk_message_type==GTK_MESSAGE_QUESTION)
    {
      gtk_window_set_title	(GTK_WINDOW(message),"Question");
      message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
    }
  else if (gtk_message_type==GTK_MESSAGE_ERROR)
    {
      gtk_window_set_title	(GTK_WINDOW(message),"Error");
      message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
    }
  else if (gtk_message_type==GTK_MESSAGE_INFO)
    {
      gtk_window_set_title	(GTK_WINDOW(message),"Information");
      message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    }
  else
    {
      gtk_window_set_title	(GTK_WINDOW(message),"Information");
      message_icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    }
		
  gtk_box_pack_start (GTK_BOX (message_hbox), message_icon, FALSE, FALSE, 0);
		
  message_right_vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (message_right_vbox), 8);
  gtk_box_pack_start (GTK_BOX (message_hbox), message_right_vbox, FALSE, FALSE, 0);
		
  message_label1 = gtk_label_new (text1);
		
  gtk_label_set_justify	(GTK_LABEL(message_label1),  GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (message_right_vbox), message_label1, FALSE, FALSE, 0);
		
  if (text2)
    {
      message_label2 = gtk_label_new (text2);
			
      gtk_label_set_justify	(GTK_LABEL(message_label2),  GTK_JUSTIFY_LEFT);
      gtk_box_pack_start (GTK_BOX (message_right_vbox), message_label2, FALSE, FALSE, 0);
    }
		
  gtk_widget_show_all (message_hbox);
		
  //GTK_BUTTONS_YES_NO
  return message;
		
}

bool dialog_message (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect=true)
{
  GtkWidget *message			 = NULL; 
  message=dialog_message_generator(gtk_message_type, text1, text2, connect);
		
  gtk_widget_show(message);
  if (connect)
    g_signal_connect_swapped (message, "response",G_CALLBACK (gtk_widget_destroy), message);
		
  return 0;
}

bool dialog_severe_error (GtkMessageType gtk_message_type, char* const text1, char* const text2, bool connect=true)
{
  GtkWidget *message			 = NULL; 
  message=dialog_message_generator(gtk_message_type, text1, text2, connect);
		
  gtk_widget_show(message);
  if (connect)
    g_signal_connect_swapped (message, "response",G_CALLBACK (destroy_main), message);
		
  return 0;
}
