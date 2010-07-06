#include "include/robotMotorGui.h"
#include "include/partMover.h"

#include <string.h>

const int UPDATE_TIME = 200;   //update time in ms

/*
 * Disable PID
 */

void partMover::dis_click(GtkButton *button, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;

  ipid->disablePid(*joint);
  iamp->disableAmp(*joint);
  return;
}

/*
 * Calibrate Joint
 */

void partMover::calib_click(GtkButton *button, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  IControlCalibration2 *ical = currentPart->cal;
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  ResourceFinder *fnd = currentPart->finder;
  //fprintf(stderr, "opening file \n");
  char buffer1[800];
  char buffer2[800];

  strcpy(buffer1, currentPart->partLabel);
  strcpy(buffer2, strcat(buffer1, "_calib"));

  if (!fnd->findGroup(buffer2).isNull())
    {
      bool ok = true;
      Bottle xtmp;

      xtmp.clear();
      xtmp = fnd->findGroup(buffer2).findGroup("CalibrationType");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      unsigned char type = (unsigned char) xtmp.get(*joint+1).asDouble();
      fprintf(stderr, "%d ", type);

      xtmp.clear();
      xtmp = fnd->findGroup(buffer2).findGroup("Calibration1");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      double param1 = xtmp.get(*joint+1).asDouble();
      fprintf(stderr, "%f ", param1);

      xtmp.clear();
      xtmp = fnd->findGroup(buffer2).findGroup("Calibration2");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      double param2 = xtmp.get(*joint+1).asDouble();
      fprintf(stderr, "%f ", param2);

      xtmp.clear();
      xtmp = fnd->findGroup(buffer2).findGroup("Calibration3");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      double param3 = xtmp.get(*joint+1).asDouble();
      fprintf(stderr, "%f \n", param3);


      if(!ok)
	dialog_message(GTK_MESSAGE_ERROR,(char *)"Check number of calib entries in the group",  buffer2, true);
      else
	ical->calibrate2(*joint, type, param1, param2, param3);
    }
  else
    dialog_message(GTK_MESSAGE_ERROR,(char *)"The supplied file does not conatain a group named:",  buffer2, true);

  return;
}

/*
 * Calibrate Joint
 */

void partMover::home_click(GtkButton *button, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  IControlCalibration2 *ical = currentPart->cal;
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  //fprintf(stderr, "Retrieving finder \n");
  ResourceFinder *fnd = currentPart->finder;
  //fprintf(stderr, "Retrieved finder: %p \n", fnd);
  char buffer1[800];
  char buffer2[800];

  strcpy(buffer1, currentPart->partLabel);
  strcpy(buffer2, strcat(buffer1, "_zero"));
  //fprintf(stderr, "Finder retrieved %s\n", buffer2);

  if (!fnd->findGroup(buffer2).isNull() && !fnd->isNull())
    {
      //fprintf(stderr, "Home group was not empty \n");
      bool ok = true;
      Bottle xtmp;
      xtmp = fnd->findGroup(buffer2).findGroup("PositionZero");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      double positionZero = xtmp.get(*joint+1).asDouble();
      //fprintf(stderr, "%f\n", positionZero);

      xtmp = fnd->findGroup(buffer2).findGroup("VelocityZero");
      //fprintf(stderr, "VALUE VEL is %d \n", fnd->findGroup(buffer2).find("VelocityZero").toString().c_str());
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      double velocityZero = xtmp.get(*joint+1).asDouble();
      //fprintf(stderr, "%f\n", velocityZero);

      if(!ok)
	dialog_message(GTK_MESSAGE_ERROR,(char *) "Check the number of entries in the group",  buffer2, true);
      else
	{
	  ipos->setRefSpeed(*joint, velocityZero);
	  ipos->positionMove(*joint, positionZero);
	}
    }
  else
    {
      //		currentPart->dialog_message(GTK_MESSAGE_ERROR,"No calib file found", strcat("Define a suitable ", strcat(currentPart->partLabel, "Calib")), true);        
      dialog_message(GTK_MESSAGE_ERROR,(char *) "No zero group found in the supplied file. Define a suitable",  buffer2, true);   
    }
  return;
}


/*
 * Enable PID and refresh position slider
 */

void partMover::run_click(GtkButton *button, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  GtkWidget **sliderAry = currentPart->sliderArray;
	 
  double posJoint;
	
  while (!iiencs->getEncoder(*joint, &posJoint))
    Time::delay(0.001);
  iamp->enableAmp(*joint);
  ipid->enablePid(*joint);
  gtk_range_set_value ((GtkRange *) (sliderAry[*joint]), posJoint);
  return;
}



/*
 * Refresh encoder position
 */

bool partMover::entry_update(partMover *currentPart)
{
  GdkColor color_grey;
  GdkColor color_yellow;
  GdkColor color_green;
  GdkColor color_red;
  GdkColor color_pink;
  GdkColor color_indaco;
  GdkColor color_white;
  GdkColor color_blue;
  
  color_pink.red=219*255;
  color_pink.green=166*255;
  color_pink.blue=171*255;

  color_red.red=255*255;
  color_red.green=100*255;
  color_red.blue=100*255;

  color_grey.red=220*255;
  color_grey.green=220*255;
  color_grey.blue=220*255;

  color_white.red=250*255;
  color_white.green=250*255;
  color_white.blue=250*255;

  color_green.red=149*255;
  color_green.green=221*255;
  color_green.blue=186*255;

  color_blue.red=150*255;
  color_blue.green=190*255;
  color_blue.blue=255*255;

  color_indaco.red=220*255;
  color_indaco.green=190*255;
  color_indaco.blue=220*255;

  color_yellow.red=249*255;
  color_yellow.green=236*255;
  color_yellow.blue=141*255;
  
  GdkColor* pColor= &color_grey;

  static int slowSwitcher = 0;

  IControlMode *ictrl = currentPart->ctrlmode;
  
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;

  GtkEntry **entry = (GtkEntry **) currentPart->currPosArray;
  GtkEntry **inEntry = (GtkEntry **) currentPart->inPosArray;
  GtkWidget **colorback = (GtkWidget **) currentPart->frameColorBack;

  GtkWidget **sliderAry = currentPart->sliderArray;
  bool *POS_UPDATE = currentPart->CURRENT_POS_UPDATE;

  char buffer[40] = {'i', 'n', 'i', 't'};
  char frame_title [255];

  double positions[MAX_NUMBER_OF_JOINTS];
  int k;
  int NUMBER_OF_JOINTS;
  bool done = false;
  static int control_mode=0;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  if (NUMBER_OF_JOINTS == 0)
  {
	  fprintf(stderr,"Lost connection with iCubInterface. You should save and restart.\n" );
	  Time::delay(0.1);
	  return true;
  }

  while(!iiencs->getEncoders(positions))
    Time::delay(0.001);
  
  //tmp
  //fprintf(stderr, "Number of joints is: %d\n", NUMBER_OF_JOINTS);
  //for (int i = 0; i < NUMBER_OF_JOINTS; i++)
  //  fprintf(stderr, "%.1f ", positions[i]);
  //fprintf(stderr, "\n");
  //endtmp

  //update all joints positions
  for (k = 0; k < NUMBER_OF_JOINTS; k++)
    {
      sprintf(buffer, "%.1f", positions[k]);  
      gtk_entry_set_text((GtkEntry*) entry[k],  buffer);
    }
  //update all joint sliders
  for (k = 0; k < NUMBER_OF_JOINTS; k++) 
    if(POS_UPDATE[k])
      gtk_range_set_value((GtkRange*)sliderAry[k],  positions[k]);
      


  //update just one checkMotion done to safe badwidth
  k = slowSwitcher%NUMBER_OF_JOINTS;
  sprintf(frame_title,"Joint %d ",k );
  slowSwitcher++;
  ipos->checkMotionDone(k, &done);

  if (!done)
      gtk_entry_set_text((GtkEntry*) inEntry[k],  " "); 
  else
      gtk_entry_set_text((GtkEntry*) inEntry[k],  "@");
  
  bool ret = ictrl->getControlMode(k, &control_mode);
  //control_mode++; if (control_mode>4) control_mode=0;
  switch (control_mode)
  {
	  case VOCAB_CM_IDLE:
		  pColor=&color_yellow;
  		  strcat(frame_title," (IDLE)");
  	      gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
	  case VOCAB_CM_POSITION:
		  pColor=&color_green;
		  strcat(frame_title," (POSITION)");
	      gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider1[k]),"Position:");
          gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider2[k]),"Velocity:");
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
	  case VOCAB_CM_VELOCITY:
		  pColor=&color_blue;
		  strcat(frame_title," (VELOCITY)");
		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
	  case VOCAB_CM_TORQUE:
		  pColor=&color_pink;
		  strcat(frame_title," (TORQUE)");
  		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider1[k]),"Torque:");
          gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider2[k]),"Torque2:");
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
		  break;
	  case VOCAB_CM_IMPEDANCE_POS:
		  pColor=&color_indaco;
		  strcat(frame_title," (IMPEDANCE POS)");
  		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
  	  case VOCAB_CM_IMPEDANCE_VEL:
		  pColor=&color_indaco;
		  strcat(frame_title," (IMPEDANCE VEL)");
  		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
	  case VOCAB_CM_OPENLOOP:
		  pColor=&color_white;
		  strcat(frame_title," (OPENLOOP)");
  		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
	  default:
	  case VOCAB_CM_UNKNOWN:
		  pColor=&color_grey;
		  strcat(frame_title," (UNKNOWN)");
		  gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
	  break;
  }

// 		  pColor=&color_blue;
//		  gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);

  IAmplifierControl *iamp =  currentPart->amp;
  
  int curr_amp_status=0;
  int amp_status[100]; //fix this!!!
  for (int i=0; i<100; i++) amp_status[i]=0;
  iamp->getAmpStatus(amp_status); //fix this!!!
  curr_amp_status=amp_status[k]; //fix this!!!
 // fprintf(stderr, "FAULT : %x %x\n", amp_status[k], amp_status[k] & 0xFF);
  if ((amp_status[k] & 0xFF)!=0)
  {
//	  fprintf(stderr, "FAULT DETECTED: %x\n", curr_amp_status);
	 pColor=&color_red;
	 strcat(frame_title," (FAULT)");
	 gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
	 gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
  }

  return true;
	
}

void partMover::enable_entry_update(partMover* currentPartMover)
{
  guint* entry_label = currentPartMover->entry_id;
  *entry_label = gtk_timeout_add(UPDATE_TIME, (GtkFunction) entry_update, currentPartMover);
  return;
}

void partMover::disable_entry_update(partMover* currentPartMover)
{
  guint* entry_label = currentPartMover->entry_id;
  if (*entry_label!=-1)
    gtk_timeout_remove(*entry_label);

  return;
}



/*
 * If the position slider is realeased
 * execute the corresponding movement
 */

void partMover::sliderVel_release(GtkRange *range, gtkClassData* currentClassData)
{	
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  GtkWidget **sliderAry = currentPart->sliderArray;

  double val = gtk_range_get_value(range);
  double posit = gtk_range_get_value((GtkRange *) sliderAry[*joint]);
  ipos->setRefSpeed(*joint, val);
  ipos->positionMove(*joint, posit);
  return;
}


/*
 * If the position slider is realeased
 * execute the corresponding movement
 */

void partMover::slider_release(GtkRange *range, gtkClassData* currentClassData)
{	
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  bool *POS_UPDATE = currentPart->CURRENT_POS_UPDATE;
  IPositionControl *ipos = currentPart->pos;
  GtkWidget **sliderVel = currentPart->sliderVelArray;

  double val = gtk_range_get_value(range);
  double valVel = gtk_range_get_value((GtkRange *)sliderVel[*joint]);

  if (!POS_UPDATE[*joint])
    {
      ipos->setRefSpeed(*joint, valVel);
      ipos->positionMove(*joint, val);
    }
  return;
}

/*
 * If the position slider is slected
 * stop updating its position
 */

void partMover::slider_pick(GtkRange *range, GdkEvent *e, gtkClassData* currentClassData)
{	
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  IPositionControl *ipos = currentPart->pos;
  bool *POS_UPDATE = currentPart->CURRENT_POS_UPDATE;

  //fprintf(stderr, "Slider_pick has been called %d \n", *joint);
  POS_UPDATE[*joint] = false;
  //partMover *currentPart = currentClassData->partPointer;

  
  return;
}

/*
 * If the position slider is realeased
 * start again updating the position
 */

void partMover::slider_unpick(GtkRange *range, GdkEvent *e, gtkClassData* currentClassData)
{	
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  bool *POS_UPDATE = currentPart->CURRENT_POS_UPDATE;
  IPositionControl *ipos = currentPart->pos;

  //fprintf(stderr, "Slider_unpick has been called %d \n", *joint);
  POS_UPDATE[*joint] = true;
  //partMover *currentPart = currentClassData->partPointer;

  
  return;
}
