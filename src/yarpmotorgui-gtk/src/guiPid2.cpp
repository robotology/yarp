#include "include/guiPid2.h"
#include "include/partMover.h"
#include <stdlib.h>


#define CAN_SET_DEBUG_PARAM_1		46
#define CAN_GET_DEBUG_PARAM_1		47
#define CAN_SET_DEBUG_PARAM_2		48
#define CAN_GET_DEBUG_PARAM_2		49
#define CAN_SET_DEBUG_PARAM_3		14
#define CAN_GET_DEBUG_PARAM_3		15
#define CAN_SET_DEBUG_PARAM_4		16
#define CAN_GET_DEBUG_PARAM_4		17

//*********************************************************************************
// This callback exits from the Pid dialog
void guiPid2::destroy_win (GtkButton *dummy1, GtkWidget *dummy2)
{
  gtk_widget_destroy (trq_winPid);
  trq_winPid=NULL;
  gtk_main_quit ();
}

//*********************************************************************************
// This callback is called when the notebook change pages
void guiPid2::change_page (GtkNotebook *notebook, GtkWidget   *page,  guint page_num,  gpointer user_data)
{
	//color definitions
	GdkColor color_green;
	GdkColor color_red;
	GdkColor color_pink;
	color_pink.red=219*255;   //DB
	color_pink.green=166*255; //A6
	color_pink.blue=171*255;  //AB
	color_red.red=255*255;     //FF
	color_red.green=100*255;    //64
	color_red.blue=100*255;
	color_green.red=149*255;   //95
	color_green.green=221*255; //DD
	color_green.blue=186*255;  //BA

	gtk_label_set_markup (GTK_LABEL (note_lbl1), g_markup_printf_escaped ("Position PID"));
    gtk_label_set_markup (GTK_LABEL (note_lbl2), g_markup_printf_escaped ("Torque PID"));
    gtk_label_set_markup (GTK_LABEL (note_lbl3), g_markup_printf_escaped ("Stiffness params"));

	if (page_num==0) gtk_label_set_markup (GTK_LABEL (note_lbl1), g_markup_printf_escaped ("<span bgcolor=\"#95DDBA\">Position PID</span>"));
    if (page_num==1) gtk_label_set_markup (GTK_LABEL (note_lbl2), g_markup_printf_escaped ("<span bgcolor=\"#FF6464\">Torque PID</span>"));
    if (page_num==2) gtk_label_set_markup (GTK_LABEL (note_lbl3), g_markup_printf_escaped ("<span bgcolor=\"#DBA6AB\">Stiffness params</span>"));

/*
	if (iPid)
		iPid->getPid(*joint, &myPosPid);
	if (iTrq)
		iTrq->getTorquePid(*joint, &myTrqPid);
	if (iImp)
		iImp->getImpedance(*joint, &stiff_val, &damp_val, &offset_val);
*/
}


//*********************************************************************************
void guiPid2::send_pos_pid (GtkButton *button, Pid *pid)
{
  char buffer[40];

  //fprintf(stderr, "%s \n", gtk_entry_get_text((GtkEntry*) kpDes));
  pid->kp = atoi(gtk_entry_get_text((GtkEntry*) pos_kpDes));
  //fprintf(stderr, "kp changed to: %d \n", (int) pid->kp);
  pid->kd = atoi(gtk_entry_get_text((GtkEntry*) pos_kdDes));
  pid->ki = atoi(gtk_entry_get_text((GtkEntry*) pos_kiDes));
  pid->scale = atoi(gtk_entry_get_text((GtkEntry*) pos_scaleDes));
  pid->offset = atoi(gtk_entry_get_text((GtkEntry*) pos_offsetDes));
  pid->max_output = atoi(gtk_entry_get_text((GtkEntry*) pos_PWM_limitDes));
  pid->max_int = atoi(gtk_entry_get_text((GtkEntry*) pos_INT_limitDes));

  //fprintf(stderr, "Trying to send pid...");
  iPid->setPid(*joint, *pid);
  //fprintf(stderr, "...got it! ");
  iPid->getPid(*joint, pid);

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
  sprintf(buffer, "%d", (int) pid->max_output);
  gtk_entry_set_text((GtkEntry*) pos_PWM_limitEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->max_int);
  gtk_entry_set_text((GtkEntry*) pos_INT_limitEntry,  buffer);

}

//*********************************************************************************
void guiPid2::send_trq_pid (GtkButton *button, Pid *pid)
{
  char buffer[40];

  //fprintf(stderr, "%s \n", gtk_entry_get_text((GtkEntry*) kpDes));
  pid->kp = atoi(gtk_entry_get_text((GtkEntry*) trq_kpDes));
  //fprintf(stderr, "kp changed to: %d \n", (int) pid->kp);
  pid->kd = atoi(gtk_entry_get_text((GtkEntry*) trq_kdDes));
  pid->ki = atoi(gtk_entry_get_text((GtkEntry*) trq_kiDes));
  pid->scale = atoi(gtk_entry_get_text((GtkEntry*) trq_scaleDes));
  pid->offset = atoi(gtk_entry_get_text((GtkEntry*) trq_offsetDes));
  pid->max_output = atoi(gtk_entry_get_text((GtkEntry*) trq_PWM_limitDes));
  pid->max_int = atoi(gtk_entry_get_text((GtkEntry*) trq_INT_limitDes));

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
  sprintf(buffer, "%d", (int) pid->max_output);
  gtk_entry_set_text((GtkEntry*) trq_PWM_limitEntry,  buffer);
  sprintf(buffer, "%d", (int) pid->max_int);
  gtk_entry_set_text((GtkEntry*) trq_INT_limitEntry,  buffer);
}

//*********************************************************************************
void guiPid2::send_imp_pid (GtkButton *button, Pid *pid)
{
  char buffer[40];

  double stiff_val=atof(gtk_entry_get_text((GtkEntry*) imp_stiffDes));
  double damp_val=atof(gtk_entry_get_text((GtkEntry*) imp_dampDes));
  double offset_val=atof(gtk_entry_get_text((GtkEntry*) imp_offDes));
  iImp->setImpedance(*joint,  stiff_val,  damp_val,  offset_val);
  iImp->getImpedance(*joint, &stiff_val, &damp_val, &offset_val);

  sprintf(buffer, "%3.3f", stiff_val);
  gtk_entry_set_text((GtkEntry*) imp_stiffEntry,  buffer);
  sprintf(buffer, "%3.3f", damp_val);
  gtk_entry_set_text((GtkEntry*) imp_dampEntry,  buffer);
  sprintf(buffer, "%3.3f", offset_val);
  gtk_entry_set_text((GtkEntry*) imp_offEntry,  buffer);
}

//*********************************************************************************
void guiPid2::send_dbg_pid (GtkButton *button, Pid *pid)
{
  if (iDbg==0) return;
  char buffer[40];

  double debug_param1 = 0;
  double debug_param2 = 0;
  double debug_param3 = 0;
  double debug_param4 = 0;
  debug_param1=atoi(gtk_entry_get_text((GtkEntry*) dbg_debug1Des));
  debug_param2=atoi(gtk_entry_get_text((GtkEntry*) dbg_debug2Des));
  debug_param3=atoi(gtk_entry_get_text((GtkEntry*) dbg_debug3Des));
  debug_param4=atoi(gtk_entry_get_text((GtkEntry*) dbg_debug4Des));

  iDbg->setParameter(*joint, CAN_SET_DEBUG_PARAM_1,  debug_param1); debug_param1 = 0;
  iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_1, &debug_param1);
  iDbg->setParameter(*joint, CAN_SET_DEBUG_PARAM_2,  debug_param2); debug_param2 = 0;
  iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_2, &debug_param2);
  iDbg->setParameter(*joint, CAN_SET_DEBUG_PARAM_3,  debug_param3); debug_param3 = 0;
  iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_3, &debug_param3);
  iDbg->setParameter(*joint, CAN_SET_DEBUG_PARAM_4,  debug_param4); debug_param4 = 0;
  iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_4, &debug_param4);

  sprintf(buffer, "%d", (int) debug_param1);
  gtk_entry_set_text((GtkEntry*) dbg_debug1Entry,  buffer);
  sprintf(buffer, "%d", (int) debug_param2);
  gtk_entry_set_text((GtkEntry*) dbg_debug2Entry,  buffer);
  sprintf(buffer, "%d", (int) debug_param3);
  gtk_entry_set_text((GtkEntry*) dbg_debug3Entry,  buffer);
  sprintf(buffer, "%d", (int) debug_param4);
  gtk_entry_set_text((GtkEntry*) dbg_debug4Entry,  buffer);
}

//*********************************************************************************
void guiPid2::displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+0, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+30, posY+20);
  gtk_widget_set_size_request 	(frame, 130, 60);
  gtk_widget_set_size_request 	(entry, 60, 20);

  gtk_editable_set_editable ((GtkEditable*) entry, FALSE);
  sprintf(buffer, "%d", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}

//*********************************************************************************
void guiPid2::displayPidValue(double k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+0, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+30, posY+20);
  gtk_widget_set_size_request 	(frame, 130, 60);
  gtk_widget_set_size_request 	(entry, 60, 20);

  gtk_editable_set_editable ((GtkEditable*) entry, FALSE);
  sprintf(buffer, "%3.3f", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}

//*********************************************************************************
void guiPid2::changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+20, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+50, posY+20);
  gtk_widget_set_size_request 	(frame, 130, 60);
  gtk_widget_set_size_request 	(entry, 60, 20);
  gtk_editable_set_editable ((GtkEditable*) entry, TRUE);
  sprintf(buffer, "%d", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}

//*********************************************************************************
void guiPid2::changePidValue(double k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label)
{
  char buffer[40];
  GtkWidget *frame = gtk_frame_new (label);

  gtk_fixed_put	(GTK_FIXED(inv), frame, posX+20, posY);
  gtk_fixed_put	(GTK_FIXED(inv), entry, posX+50, posY+20);
  gtk_widget_set_size_request 	(frame, 130, 60);
  gtk_widget_set_size_request 	(entry, 60, 20);
  gtk_editable_set_editable ((GtkEditable*) entry, TRUE);
  sprintf(buffer, "%3.3f", k);
  gtk_entry_set_text((GtkEntry*) entry,  buffer);
  return;
}
//*********************************************************************************
void guiPid2::guiPid2(void *button, void* data)
{
  int*        new_joint = ((gtkClassData*)(data))->indexPointer;
  partMover * new_part  = ((gtkClassData*)(data))->partPointer;

  if (joint!=0 && (*new_joint != *joint || new_part != currentPart))
  {
	  if (trq_winPid != NULL) destroy_win(NULL, NULL);
  }

  if (trq_winPid != NULL) 
  {
	  gtk_window_set_keep_above    (GTK_WINDOW(trq_winPid),true);
	  return;
  }

  gtkClassData* currentClassData = (gtkClassData*) data;
  currentPart = currentClassData->partPointer;
  joint  = currentClassData->indexPointer;
  iPid  = currentPart->get_IPidControl();
  iTrq  = currentPart->get_ITorqueControl();
  iImp  = currentPart->get_IImpedanceControl();
  iDbg  = currentPart->get_IDebugControl();

  //GtkWidget *winPid = NULL;

  GtkWidget *button_Pos_Send;
  GtkWidget *button_Pos_Close;
  GtkWidget *button_Trq_Send;
  GtkWidget *button_Trq_Close;
  GtkWidget *button_Imp_Send;
  GtkWidget *button_Imp_Close;
  GtkWidget *button_Dbg_Send;
  GtkWidget *button_Dbg_Close;
  Pid myPosPid(0, 0, 0, 0, 0, 0);
  Pid myTrqPid(0, 0, 0, 0, 0, 0);
  double stiff_val=0;
  double damp_val=0;
  double offset_val=0;
  double debug_param1 = 0;
  double debug_param2 = 0;
  double debug_param3 = 0;
  double debug_param4 = 0;

  iPid->getPid(*joint, &myPosPid);
  iTrq->getTorquePid(*joint, &myTrqPid);
  iImp->getImpedance(*joint, &stiff_val, &damp_val, &offset_val);
  if (iDbg != 0)
  {
	iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_1, &debug_param1);
	iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_2, &debug_param2);
	iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_3, &debug_param3);
	iDbg->getParameter(*joint, CAN_GET_DEBUG_PARAM_4, &debug_param4);
  }

#if 0
  //ImpedanceOffset test
  iImp->setImpedanceOffset(*joint, 0.2); 
  iImp->getImpedanceOffset(*joint, &offset_val);
#endif

  //adding a popup window
  trq_winPid = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  char title[255];
  sprintf(title,"Pid control %s JNT:%d",currentPart->partLabel, *joint);
  gtk_window_set_title (GTK_WINDOW (trq_winPid), title);

  //adding a notebook
  note_book = gtk_notebook_new ();
  //adding notebook labels
  note_lbl1 = gtk_label_new("Position PID");
  note_lbl2 = gtk_label_new("Torque PID");
  note_lbl3 = gtk_label_new("Stiffness params");
  note_lbl4 = gtk_label_new("Other");

  note_pag1 = gtk_fixed_new ();
  note_pag2 = gtk_fixed_new ();
  note_pag3 = gtk_fixed_new ();
  note_pag4 = gtk_fixed_new ();

  gtk_notebook_append_page   (GTK_NOTEBOOK(note_book), note_pag1, note_lbl1);
  gtk_notebook_append_page   (GTK_NOTEBOOK(note_book), note_pag2, note_lbl2);
  gtk_notebook_append_page   (GTK_NOTEBOOK(note_book), note_pag3, note_lbl3);
  gtk_notebook_append_page   (GTK_NOTEBOOK(note_book), note_pag4, note_lbl4);

  //adding a set of display
  //inv = gtk_fixed_new ();

  gtk_container_add (GTK_CONTAINER (trq_winPid), note_book);

  // ------ DEBUG CONTROL ------
  //debug_param1
  dbg_debug1Entry   =  gtk_entry_new();
  displayPidValue((int) debug_param1, note_pag4, dbg_debug1Entry, 0, 0, "Current Debug1");
  //debug_param1 desired
  dbg_debug1Des   =  gtk_entry_new();
  changePidValue((int) debug_param1, note_pag4, dbg_debug1Des, 110, 0, "Desired Debug1");
  //debug_param2
  dbg_debug2Entry   =  gtk_entry_new();
  displayPidValue((int) debug_param2, note_pag4, dbg_debug2Entry, 0, 70, "Current Debug2");
  //debug_param2 desired
  dbg_debug2Des   =  gtk_entry_new();
  changePidValue((int) debug_param2, note_pag4, dbg_debug2Des, 110, 70, "Desired Debug2");
  //debug_param1
  dbg_debug3Entry   =  gtk_entry_new();
  displayPidValue((int) debug_param3, note_pag4, dbg_debug3Entry, 0, 140, "Current debug3");
  //debug_param1 desired
  dbg_debug3Des   =  gtk_entry_new();
  changePidValue((int) debug_param3, note_pag4, dbg_debug3Des, 110, 140, "Desired debug3");
  //debug_param2
  dbg_debug4Entry   =  gtk_entry_new();
  displayPidValue((int) debug_param4, note_pag4, dbg_debug4Entry, 0, 210, "Current debug4");
  //debug_param2 desired
  dbg_debug4Des   =  gtk_entry_new();
  changePidValue((int) debug_param4, note_pag4, dbg_debug4Des, 110, 210, "Desired debug4");

  // ------ POSITION CONTROL ------
  //kp
  pos_kpEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.kp, note_pag1, pos_kpEntry, 0, 0, "Current Position Kp");
  //kp desired
  pos_kpDes   =  gtk_entry_new();
  changePidValue((int) myPosPid.kp, note_pag1, pos_kpDes, 110, 0, "Desired Position Kp");
  //kd
  pos_kdEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.kd, note_pag1, pos_kdEntry, 0, 70, "Current Position Kd");
  //kd desired
  pos_kdDes   =  gtk_entry_new();
  changePidValue((int) myPosPid.kd, note_pag1, pos_kdDes, 110, 70, "Desired Position Kd");
  //ki
  pos_kiEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.ki, note_pag1, pos_kiEntry, 0, 140, "Current Position Ki");
  //ki desired
  pos_kiDes   =  gtk_entry_new();
  changePidValue((int) myPosPid.ki, note_pag1, pos_kiDes, 110, 140, "Desired Position Ki");
  //scale
  pos_scaleEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.scale, note_pag1, pos_scaleEntry, 0, 210, "Current Position shift");
  //scale desired
  pos_scaleDes   =  gtk_entry_new();
  changePidValue((int) myPosPid.scale, note_pag1, pos_scaleDes, 110, 210, "Desired Position shift");
  //offset
  pos_offsetEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.offset, note_pag1, pos_offsetEntry, 0, 280, "Current Position offset");
  //offset desired
  pos_offsetDes   =  gtk_entry_new();
  changePidValue((int) myPosPid.offset, note_pag1, pos_offsetDes, 110, 280, "Desired Position offset");

  //PWM limit
  pos_PWM_limitEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.max_output, note_pag1, pos_PWM_limitEntry, 0, 360, "Current PWM limit");
  //PWM limit desired
  pos_PWM_limitDes=  gtk_entry_new();
  changePidValue((int) myPosPid.max_output, note_pag1, pos_PWM_limitDes, 110, 360, "Desired PWM limit");
  //INTEGRAL limit
  pos_INT_limitEntry   =  gtk_entry_new();
  displayPidValue((int) myPosPid.max_int, note_pag1, pos_INT_limitEntry, 0, 430, "Current Integral limit");
  //INTEGRAL limit desired
  pos_INT_limitDes=  gtk_entry_new();
  changePidValue((int) myPosPid.max_int, note_pag1, pos_INT_limitDes, 110, 430, "Desired Integral limit");

    // ------ TORQUE CONTROL ------
  //kp
  trq_kpEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.kp, note_pag2, trq_kpEntry, 0, 0, "Current Torque Kp");
  //kp desired
  trq_kpDes   =  gtk_entry_new();
  changePidValue((int) myTrqPid.kp, note_pag2, trq_kpDes, 110, 0, "Desired Torque Kp");
  //kd
  trq_kdEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.kd, note_pag2, trq_kdEntry, 0, 70, "Current Torque Kd");
  //kd desired
  trq_kdDes   =  gtk_entry_new();
  changePidValue((int) myTrqPid.kd, note_pag2, trq_kdDes, 110, 70, "Desired Torque Kd");
  //ki
  trq_kiEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.ki, note_pag2, trq_kiEntry, 0, 140, "Current Torque Ki");
  //ki desired
  trq_kiDes   =  gtk_entry_new();
  changePidValue((int) myTrqPid.ki, note_pag2, trq_kiDes, 110, 140, "Desired Torque Ki");
  //scale
  trq_scaleEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.scale, note_pag2, trq_scaleEntry, 0, 210, "Current Torque shift");
  //scale desired
  trq_scaleDes   =  gtk_entry_new();
  changePidValue((int) myTrqPid.scale, note_pag2, trq_scaleDes, 110, 210, "Desired Torque shift");
  //offset
  trq_offsetEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.offset, note_pag2, trq_offsetEntry, 0, 280, "Current Torque offset");
  //offset desired
  trq_offsetDes   =  gtk_entry_new();
  changePidValue((int) myTrqPid.offset, note_pag2, trq_offsetDes, 110, 280, "Desired Torque offset");

  //PWM limit
  trq_PWM_limitEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.max_output, note_pag2, trq_PWM_limitEntry, 0, 360, "Current PWM limit");
  //PWM limit desired
  trq_PWM_limitDes=  gtk_entry_new();
  changePidValue((int) myTrqPid.max_output, note_pag2, trq_PWM_limitDes, 110, 360, "Desired PWM limit");
  //INTEGRAL limit
  trq_INT_limitEntry   =  gtk_entry_new();
  displayPidValue((int) myTrqPid.max_int, note_pag2, trq_INT_limitEntry, 0, 430, "Current Integral limit");
  //INTEGRAL limit desired
  trq_INT_limitDes=  gtk_entry_new();
  changePidValue((int) myTrqPid.max_int, note_pag2, trq_INT_limitDes, 110, 430, "Desired Integral limit");

  // ------ IMPEDANCE CONTROL ------
  //stiffness
  imp_stiffEntry = gtk_entry_new();
  displayPidValue(stiff_val, note_pag3, imp_stiffEntry, 0, 0, "Current Joint stiffness");
  //stiffness desired
  imp_stiffDes   =  gtk_entry_new();
  changePidValue(stiff_val, note_pag3, imp_stiffDes, 110, 0, "Desired Joint stiffness");

  //damping
  imp_dampEntry = gtk_entry_new();
  displayPidValue(damp_val, note_pag3, imp_dampEntry, 0, 70, "Current Joint damping");
  //damping desired
  imp_dampDes   =  gtk_entry_new();
  changePidValue(damp_val, note_pag3, imp_dampDes, 110, 70, "Desired Joint damping");

  //offset
  imp_offEntry = gtk_entry_new();
  displayPidValue(offset_val, note_pag3, imp_offEntry, 0, 140, "Current Joint Force off");
  //offset desired
  imp_offDes   =  gtk_entry_new();
  changePidValue(offset_val, note_pag3, imp_offDes, 110, 140, "Desired Joint Force off");

  //Send buttons
  button_Pos_Send = gtk_button_new_with_mnemonic ("Send");
  button_Trq_Send = gtk_button_new_with_mnemonic ("Send");
  button_Imp_Send = gtk_button_new_with_mnemonic ("Send");
  button_Dbg_Send = gtk_button_new_with_mnemonic ("Send");
  gtk_fixed_put	(GTK_FIXED(note_pag1), button_Pos_Send, 0, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag2), button_Trq_Send, 0, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag3), button_Imp_Send, 0, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag4), button_Dbg_Send, 0, 520);
  g_signal_connect (button_Pos_Send, "clicked", G_CALLBACK (send_pos_pid), &myPosPid);
  g_signal_connect (button_Trq_Send, "clicked", G_CALLBACK (send_trq_pid), &myTrqPid);
  g_signal_connect (button_Imp_Send, "clicked", G_CALLBACK (send_imp_pid), NULL);
  g_signal_connect (button_Dbg_Send, "clicked", G_CALLBACK (send_dbg_pid), NULL);
  gtk_widget_set_size_request     (button_Pos_Send, 120, 25);
  gtk_widget_set_size_request     (button_Trq_Send, 120, 25);
  gtk_widget_set_size_request     (button_Imp_Send, 120, 25);
  gtk_widget_set_size_request     (button_Dbg_Send, 120, 25);

  //Close
  button_Pos_Close = gtk_button_new_with_mnemonic ("Close");
  button_Trq_Close = gtk_button_new_with_mnemonic ("Close");
  button_Imp_Close = gtk_button_new_with_mnemonic ("Close");
  button_Dbg_Close = gtk_button_new_with_mnemonic ("Close");
  gtk_fixed_put	(GTK_FIXED(note_pag1), button_Pos_Close, 120, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag2), button_Trq_Close, 120, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag3), button_Imp_Close, 120, 520);
  gtk_fixed_put	(GTK_FIXED(note_pag4), button_Dbg_Close, 120, 520);
  g_signal_connect (button_Pos_Close, "clicked", G_CALLBACK (destroy_win), NULL);
  g_signal_connect (button_Trq_Close, "clicked", G_CALLBACK (destroy_win), NULL);
  g_signal_connect (button_Imp_Close, "clicked", G_CALLBACK (destroy_win), NULL);
  g_signal_connect (button_Dbg_Close, "clicked", G_CALLBACK (destroy_win), NULL);
  gtk_widget_set_size_request     (button_Pos_Close, 120, 25);
  gtk_widget_set_size_request     (button_Trq_Close, 120, 25);
  gtk_widget_set_size_request     (button_Imp_Close, 120, 25);
  gtk_widget_set_size_request     (button_Dbg_Close, 120, 25);

  //connection to the destroyer
  g_signal_connect (trq_winPid, "destroy",G_CALLBACK (destroy_win), NULL);
  //connect to the page switcher
  g_signal_connect (note_book, "switch-page", G_CALLBACK (change_page), NULL);

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
