#include "include/guiPidTrq.h"
#include "include/partMover.h"
#include <stdlib.h>


//*********************************************************************************
void guiPidTrq::destroy_main (GtkWindow *window,	gpointer   user_data)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  window = NULL;
  trq_winPid = NULL;
  gtk_main_quit ();
}

//*********************************************************************************
// This callback exits from the Pid dialog
void guiPidTrq::destroy_win (GtkButton *button, GtkWindow *window)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  window = NULL;
  trq_winPid = NULL;
  gtk_main_quit ();
}

//*********************************************************************************
void guiPidTrq::send_pid (GtkButton *button, Pid *pid)
{
  char buffer[40];

  //fprintf(stderr, "%s \n", gtk_entry_get_text((GtkEntry*) kpDes));
  pid->kp = atoi(gtk_entry_get_text((GtkEntry*) trq_kpDes));
  //fprintf(stderr, "kp changed to: %d \n", (int) pid->kp);
  pid->kd = atoi(gtk_entry_get_text((GtkEntry*) trq_kdDes));
  pid->ki = atoi(gtk_entry_get_text((GtkEntry*) trq_kiDes));
  pid->scale = atoi(gtk_entry_get_text((GtkEntry*) trq_scaleDes));
  pid->offset = atoi(gtk_entry_get_text((GtkEntry*) trq_offsetDes));

  //fprintf(stderr, "Trying to send pid...");
  iTrq->setTorquePid(*joint, *pid);
  //fprintf(stderr, "...got it! ");
  iTrq->getTorquePid(*joint, pid);

  sprintf(buffer, "%d", (int) pid->kp);
  gtk_entry_set_text((GtkEntry*) trq_kpEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->kd);
  gtk_entry_set_text((GtkEntry*) trq_kdEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->ki);
  gtk_entry_set_text((GtkEntry*) trq_kiEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->scale);
  gtk_entry_set_text((GtkEntry*) trq_scaleEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->offset);
  gtk_entry_set_text((GtkEntry*) trq_offsetEntry,  buffer);

  double stiff_val=atoi(gtk_entry_get_text((GtkEntry*) imp_stiffDes));
  double damp_val=atoi(gtk_entry_get_text((GtkEntry*) imp_dampDes));
  double offset_val=0;
  iImp->setImpedance(*joint,  stiff_val,  damp_val,  offset_val);
  iImp->getImpedance(*joint, &stiff_val, &damp_val, &offset_val);

  sprintf(buffer, "%d", (int) stiff_val);
  gtk_entry_set_text((GtkEntry*) imp_stiffEntry,  buffer);
  sprintf(buffer, "%d", (int) damp_val);
  gtk_entry_set_text((GtkEntry*) imp_dampEntry,  buffer);
}

//*********************************************************************************
void guiPidTrq::displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label)
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
void guiPidTrq::changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label)
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
void guiPidTrq::guiPidTrq(void *button, void* data)
{
  if (trq_winPid != NULL) 
  {
	  gtk_window_set_keep_above    (GTK_WINDOW(trq_winPid),true);
	  return;
  }

  gtkClassData* currentClassData = (gtkClassData*) data;
  partMover *currentPart = currentClassData->partPointer;
  joint  = currentClassData->indexPointer;
  iTrq  = currentPart->get_ITorqueControl();
  iImp  = currentPart->get_IImpedanceControl();

  //GtkWidget *winPid = NULL;
  GtkWidget *inv    = NULL;
  GtkWidget *buttonSend;
  GtkWidget *buttonClose;
  Pid myPid(0, 0, 0, 0, 0, 0);
  double stiff_val=0;
  double damp_val=0;
  double offset_val=0;

  iTrq->getTorquePid(*joint, &myPid);
  iImp->getImpedance(*joint, &stiff_val, &damp_val, &offset_val);

  //adding a popup window
  trq_winPid = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  char title[255];
  sprintf(title,"Pid control JNT:%d",*joint);
  gtk_window_set_title (GTK_WINDOW (trq_winPid), title);
  
  //adding a set of display
  inv = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (trq_winPid), inv);
  //kp
  trq_kpEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.kp, inv, trq_kpEntry, 0, 0, "Current Torque Kp");
  //kp desired
  trq_kpDes   =  gtk_entry_new();
  changePidValue((int) myPid.kp, inv, trq_kpDes, 100, 0, "Desired Torque Kp");
  //kd
  trq_kdEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.kd, inv, trq_kdEntry, 0, 70, "Current Torque Kd");
  //kd desired
  trq_kdDes   =  gtk_entry_new();
  changePidValue((int) myPid.kd, inv, trq_kdDes, 100, 70, "Desired Torque Kd");
  //ki
  trq_kiEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.ki, inv, trq_kiEntry, 0, 140, "Current Torque Ki");
  //ki desired
  trq_kiDes   =  gtk_entry_new();
  changePidValue((int) myPid.ki, inv, trq_kiDes, 100, 140, "Desired Torque Ki");
  //scale
  trq_scaleEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.scale, inv, trq_scaleEntry, 0, 210, "Current Torque shift");
  //scale desired
  trq_scaleDes   =  gtk_entry_new();
  changePidValue((int) myPid.scale, inv, trq_scaleDes, 100, 210, "Desired Torque shift");
  //offset
  trq_offsetEntry   =  gtk_entry_new();
  displayPidValue((int) myPid.offset, inv, trq_offsetEntry, 0, 280, "Current Torque offset");
  //offset desired
  trq_offsetDes   =  gtk_entry_new();
  changePidValue((int) myPid.offset, inv, trq_offsetDes, 100, 280, "Desired Torque offset");

  //stiffness
  imp_stiffEntry = gtk_entry_new();
  displayPidValue((int) stiff_val, inv, imp_stiffEntry, 0, 360, "Current Joint stiffness");
  //stiffness desired
  imp_stiffDes   =  gtk_entry_new();
  changePidValue((int) stiff_val, inv, imp_stiffDes, 100, 360, "Desired Joint stiffness");

  //damping
  imp_dampEntry = gtk_entry_new();
  displayPidValue((int) damp_val, inv, imp_dampEntry, 0, 430, "Current Joint damping");
  //damping desired
  imp_dampDes   =  gtk_entry_new();
  changePidValue((int) damp_val, inv, imp_dampDes, 100, 430, "Desired Joint damping");

  //Send
  buttonSend = gtk_button_new_with_mnemonic ("Send");
  gtk_fixed_put	(GTK_FIXED(inv), buttonSend, 0, 520);
  g_signal_connect (buttonSend, "clicked", G_CALLBACK (send_pid), &myPid);
  gtk_widget_set_size_request     (buttonSend, 120, 25);

  //Close
  buttonClose = gtk_button_new_with_mnemonic ("Close");
  gtk_fixed_put	(GTK_FIXED(inv), buttonClose, 120, 520);
  g_signal_connect (buttonClose, "clicked", G_CALLBACK (destroy_win), trq_winPid);
  gtk_widget_set_size_request     (buttonClose, 120, 25);

  //connection to the destroyer
  g_signal_connect (trq_winPid, "destroy",G_CALLBACK (destroy_main), &trq_winPid);
  //make it visible
  if (!GTK_WIDGET_VISIBLE (trq_winPid))
    gtk_widget_show_all (trq_winPid);
  else
    {
      gtk_widget_destroy (trq_winPid);
      trq_winPid = NULL;
    }
	
  gtk_window_set_keep_above    (GTK_WINDOW(trq_winPid),true);
  gtk_main ();

  return;
}
