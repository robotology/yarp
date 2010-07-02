#include "include/guiPid.h"
#include "include/partMover.h"
#include <stdlib.h>

//*********************************************************************************
void guiPid::destroy_main (GtkWindow *window,	gpointer   user_data)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  window = NULL;
  pos_winPid = NULL;
  gtk_main_quit ();
}

//*********************************************************************************
// This callback exits from the Pid dialog
void guiPid::destroy_win (GtkButton *button, GtkWindow *window)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  window = NULL;
  pos_winPid = NULL;
  gtk_main_quit ();
}

//*********************************************************************************
void guiPid::send_pid (GtkButton *button, Pid *pid)
{
  char buffer[40];

  //fprintf(stderr, "%s \n", gtk_entry_get_text((GtkEntry*) kpDes));
  pid->kp = atoi(gtk_entry_get_text((GtkEntry*) pos_kpDes));
  //fprintf(stderr, "kp changed to: %d \n", (int) pid->kp);
  pid->kd = atoi(gtk_entry_get_text((GtkEntry*) pos_kdDes));
  pid->ki = atoi(gtk_entry_get_text((GtkEntry*) pos_kiDes));
  pid->scale = atoi(gtk_entry_get_text((GtkEntry*) pos_scaleDes));
  pid->offset = atoi(gtk_entry_get_text((GtkEntry*) pos_offsetDes));

  //fprintf(stderr, "Trying to send pid...");
  ipid->setPid(*joint, *pid);
  //fprintf(stderr, "...got it! ");
  ipid->getPid(*joint, pid);

  sprintf(buffer, "%d", (int) pid->kp);
  gtk_entry_set_text((GtkEntry*) pos_kpEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->kd);
  gtk_entry_set_text((GtkEntry*) pos_kdEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->ki);
  gtk_entry_set_text((GtkEntry*) pos_kiEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->scale);
  gtk_entry_set_text((GtkEntry*) pos_scaleEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->offset);
  gtk_entry_set_text((GtkEntry*) pos_offsetEntry,  buffer);
}

//*********************************************************************************
void guiPid::displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+0, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+30, posY+20);
  gtk_widget_set_size_request 	(frame, 120, 60);
  gtk_widget_set_size_request 	(entry, 50, 20);

  gtk_editable_set_editable ((GtkEditable*) entry, FALSE);
  sprintf(buffer, "%d", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}

//*********************************************************************************
void guiPid::changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+20, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+50, posY+20);
  gtk_widget_set_size_request 	(frame, 120, 60);
  gtk_widget_set_size_request 	(entry, 50, 20);
  gtk_editable_set_editable ((GtkEditable*) entry, TRUE);
  sprintf(buffer, "%d", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}

//*********************************************************************************
void guiPid::guiPid(void *button, void* data)
{
  if (pos_winPid != NULL) 
  {
	  gtk_window_set_keep_above    (GTK_WINDOW(pos_winPid),true);
	  return;
  }

  gtkClassData* currentClassData = (gtkClassData*) data;
  partMover *currentPart = currentClassData->partPointer;
  joint  = currentClassData->indexPointer;
  ipid   = currentPart->get_IPidControl();

  //GtkWidget *winPid = NULL;
  GtkWidget *inv    = NULL;
  GtkWidget *buttonSend;
  GtkWidget *buttonClose;
  Pid myPid(0, 0, 0, 0, 0, 0);

  ipid->getPid(*joint, &myPid);

  //adding a popup window
  pos_winPid = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  char title[255];
  sprintf(title,"Pid control JNT:%d",*joint);
  gtk_window_set_title (GTK_WINDOW (pos_winPid), title);
  
  //adding a set of display
  inv = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (pos_winPid), inv);
  //kp
  pos_kpEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.kp, inv, pos_kpEntry, 0, 0, "Current Kp");
  //kp desired
  pos_kpDes   =  gtk_entry_new();
  changePidValue((int) myPid.kp, inv, pos_kpDes, 100, 0, "Desired Kp");
  //kd
  pos_kdEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.kd, inv, pos_kdEntry, 0, 70, "Current Kd");
  //kd desired
  pos_kdDes   =  gtk_entry_new();
  changePidValue((int) myPid.kd, inv, pos_kdDes, 100, 70, "Desired Kd");
  //ki
  pos_kiEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.ki, inv, pos_kiEntry, 0, 140, "Current Ki");
  //ki desired
  pos_kiDes   =  gtk_entry_new();
  changePidValue((int) myPid.ki, inv, pos_kiDes, 100, 140, "Desired Ki");
  //scale
  pos_scaleEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.scale, inv, pos_scaleEntry, 0, 210, "Current shift");
  //scale desired
  pos_scaleDes   =  gtk_entry_new();
  changePidValue((int) myPid.scale, inv, pos_scaleDes, 100, 210, "Desired shift");
  //offset
  pos_offsetEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.offset, inv, pos_offsetEntry, 0, 280, "Current offset");
  //offset desired
  pos_offsetDes   =  gtk_entry_new();
  changePidValue((int) myPid.offset, inv, pos_offsetDes, 100, 280, "Desired offset");


  //Send
  buttonSend = gtk_button_new_with_mnemonic ("Send");
  gtk_fixed_put	(GTK_FIXED(inv), buttonSend, 0, 350);
  g_signal_connect (buttonSend, "clicked", G_CALLBACK (send_pid), &myPid);
  gtk_widget_set_size_request     (buttonSend, 100, 25);

  //Close
  buttonClose = gtk_button_new_with_mnemonic ("Close");
  gtk_fixed_put	(GTK_FIXED(inv), buttonClose, 120, 350);
  g_signal_connect (buttonClose, "clicked", G_CALLBACK (destroy_win), pos_winPid);
  gtk_widget_set_size_request     (buttonClose, 120, 25);

  //connection to the destroyer
  g_signal_connect (pos_winPid, "destroy",G_CALLBACK (destroy_main), &pos_winPid);
  //make it visible
  if (!GTK_WIDGET_VISIBLE (pos_winPid))
    gtk_widget_show_all (pos_winPid);
  else
    {
      gtk_widget_destroy (pos_winPid);
      pos_winPid = NULL;
    }
	
  gtk_window_set_keep_above    (GTK_WINDOW(pos_winPid),true);
  gtk_main ();

  return;
}
