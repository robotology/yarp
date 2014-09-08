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


#include "include/robotMotorGui.h"
#include "include/partMover.h"
#include "include/windowTools.h"
#include <string.h>

const int UPDATE_TIME = 200;   //update time in ms
#define DEBUG_GUI 0

extern bool position_direct_enabled;
extern bool old_impedance_enabled;

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
  IControlMode2 *ictrl = currentPart->ctrlmode2;

  #if 0
  ictrl->setControlMode(*joint,VOCAB_CM_IDLE);
  #else
  ictrl->setControlMode(*joint,VOCAB_CM_FORCE_IDLE);
  #endif
  
  return;
}

/*
 * Calibrate Joint
 */

void partMover::calib_click(GtkButton *button, gtkClassData* currentClassData)
{
  //ask for confirmation
  if (!dialog_question("Do you really want to recalibrate the joint?")) 
  {
     return;
  }

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
      //        currentPart->dialog_message(GTK_MESSAGE_ERROR,"No calib file found", strcat("Define a suitable ", strcat(currentPart->partLabel, "Calib")), true);        
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
  IControlMode2 *ictrl = currentPart->ctrlmode2;
  GtkWidget **sliderAry = currentPart->sliderArray;
     
  double posJoint;
    
  while (!iiencs->getEncoder(*joint, &posJoint))
    Time::delay(0.001);
  
  ictrl->setControlMode(*joint,VOCAB_CM_POSITION);

  gtk_range_set_value ((GtkRange *) (sliderAry[*joint]), posJoint);
  return;
}



/*
 * Refresh encoder position
 */

bool partMover::entry_update(partMover *currentPart)
{
  GdkColor color_black;
  GdkColor color_grey;
  GdkColor color_yellow;
  GdkColor color_green;
  GdkColor color_green_blue;
  GdkColor color_dark_green;
  GdkColor color_red;
  GdkColor color_fault_red;
  GdkColor color_pink;
  GdkColor color_indaco;
  GdkColor color_white;
  GdkColor color_blue;
  
  color_pink.red=219*255;
  color_pink.green=166*255;
  color_pink.blue=171*255;

  color_fault_red.red=255*255;
  color_fault_red.green=10*255;
  color_fault_red.blue=10*255;

  color_black.red=10*255;
  color_black.green=10*255;
  color_black.blue=10*255;

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

  color_dark_green.red=(149-30)*255;
  color_dark_green.green=(221-30)*255;
  color_dark_green.blue=(186-30)*255;

  color_blue.red=150*255;
  color_blue.green=190*255;
  color_blue.blue=255*255;

  color_green_blue.red=(149+150)/2*255;
  color_green_blue.green=(221+190)/2*255;
  color_green_blue.blue=(186+255)/2*255;

  color_indaco.red=220*255;
  color_indaco.green=190*255;
  color_indaco.blue=220*255;

  color_yellow.red=249*255;
  color_yellow.green=236*255;
  color_yellow.blue=141*255;
  
  GdkColor* pColor= &color_grey;

  static int slowSwitcher = 0;

  IControlMode     *ictrl = currentPart->ctrlmode2;
  IInteractionMode *iint  = currentPart->iinteract;
  IPositionControl  *ipos = currentPart->pos;
  IVelocityControl  *ivel = currentPart->iVel;
  IPositionDirect   *iDir = currentPart->iDir;
  IEncoders       *iiencs = currentPart->iencs;
  ITorqueControl    *itrq = currentPart->trq;
  IAmplifierControl *iamp = currentPart->amp;

  GtkEntry * *pos_entry   = (GtkEntry **)  currentPart->currPosArray;
  GtkEntry  **trq_entry   = (GtkEntry **)  currentPart->currTrqArray;
  GtkEntry  **speed_entry = (GtkEntry **)  currentPart->currSpeedArray;
  GtkEntry    **inEntry   = (GtkEntry **)  currentPart->inPosArray;
  GtkWidget **colorback   = (GtkWidget **) currentPart->frameColorBack;

  GtkWidget **sliderAry = currentPart->sliderArray;
  bool *POS_UPDATE = currentPart->CURRENT_POS_UPDATE;

  char buffer[40] = {'i', 'n', 'i', 't'};
  char frame_title [255];

  double positions[MAX_NUMBER_OF_JOINTS];
  double torques[MAX_NUMBER_OF_JOINTS];
  double speeds[MAX_NUMBER_OF_JOINTS];
  double max_torques[MAX_NUMBER_OF_JOINTS];
  double min_torques[MAX_NUMBER_OF_JOINTS];
  static int controlModes[MAX_NUMBER_OF_JOINTS];
  static int controlModesOld[MAX_NUMBER_OF_JOINTS];
  static yarp::dev::InteractionModeEnum interactionModes[MAX_NUMBER_OF_JOINTS];
  static yarp::dev::InteractionModeEnum interactionModesOld[MAX_NUMBER_OF_JOINTS];

  int k;
  int NUMBER_OF_JOINTS=0;
  bool done = false;
  bool ret = false;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  if (NUMBER_OF_JOINTS == 0)
  {
      fprintf(stderr,"Lost connection with iCubInterface. You should save and restart.\n" );
      Time::delay(0.1);
      pColor=&color_grey;
      strcpy(frame_title,"DISCONNECTED");
      for (k = 0; k < MAX_NUMBER_OF_JOINTS; k++)
      {   
          if (currentPart->framesArray[k]!=0)
          {
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
          }
      }
      return true;
  }

  for (k = 0; k < NUMBER_OF_JOINTS; k++) 
  {
      max_torques[k]=0;
      min_torques[k]=0;
      torques[k]=0;
  }

  if (!iiencs->getEncoders(positions)) 
      return true;
  itrq->getTorques(torques);
  iiencs->getEncoderSpeeds(speeds);
  
  //update all joints positions
  for (k = 0; k < NUMBER_OF_JOINTS; k++)
    {
      sprintf(buffer, "%.1f", positions[k]);  
      gtk_entry_set_text((GtkEntry*) pos_entry[k],  buffer);
      sprintf(buffer, "%.3f", torques[k]);  
      gtk_entry_set_text((GtkEntry*) trq_entry[k],  buffer);
      sprintf(buffer, "%.1f", speeds[k]);  
      gtk_entry_set_text((GtkEntry*) speed_entry[k],  buffer);
    }
  //update all joint sliders
  for (k = 0; k < NUMBER_OF_JOINTS; k++) 
    if(POS_UPDATE[k])
      gtk_range_set_value((GtkRange*)sliderAry[k],  positions[k]);

  // *** update the checkMotionDone box section ***
  // (only one at a time in order to save badwidth)
  k = slowSwitcher%NUMBER_OF_JOINTS;
  slowSwitcher++;
#if DEBUG_GUI
  gtk_entry_set_text((GtkEntry*) inEntry[k],  "off");
#else
  ipos->checkMotionDone(k, &done);
  if (!done)
      gtk_entry_set_text((GtkEntry*) inEntry[k],  " "); 
  else
      gtk_entry_set_text((GtkEntry*) inEntry[k],  "@");
#endif

  // *** update the controlMode section ***
  // the new icubinterface does not increase the bandwidth consumption
  // ret = true; useless guys!
  ret=ictrl->getControlModes(controlModes);
  if (ret==false) fprintf(stderr,"ictrl->getControlMode failed\n" );
  ret=iint->getInteractionModes(interactionModes);
  if (ret==false) fprintf(stderr,"iint->getInteractionlMode failed\n" );

  for (k = 0; k < NUMBER_OF_JOINTS; k++)
  {
      if (currentPart->first_time==false && controlModes[k] == controlModesOld[k]) continue;
      controlModesOld[k]=controlModes[k];
      sprintf(frame_title,"Joint %d ",k );

      switch (controlModes[k])
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
          case VOCAB_CM_POSITION_DIRECT:
              pColor=&color_dark_green;
              strcat(frame_title," (POSITION_DIRECT)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider1[k]),"Position:");
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider2[k]),"---");
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
          break;
          case VOCAB_CM_MIXED:
              pColor=&color_green_blue;
              strcat(frame_title," (MIXED_MODE)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider1[k]),"Position:");
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider2[k]),"Velocity");
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
          case VOCAB_CM_HW_FAULT:
              pColor=&color_fault_red;
              strcat(frame_title," (HARDWARE_FAULT)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider1[k]),"---");
              gtk_frame_set_label   (GTK_FRAME(currentPart->frame_slider2[k]),"---");
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
              break;
         case VOCAB_CM_CALIBRATING:
              pColor=&color_grey;
              strcat(frame_title," (CALIBRATING)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
          break;
         case VOCAB_CM_CALIB_DONE:
              pColor=&color_grey;
              strcat(frame_title," (CALIB DONE)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
          break;
          case VOCAB_CM_NOT_CONFIGURED:
              pColor=&color_grey;
              strcat(frame_title," (NOT CONFIGURED)");
              gtk_frame_set_label   (GTK_FRAME(currentPart->framesArray[k]),frame_title);
              gtk_widget_modify_bg (colorback[k], GTK_STATE_NORMAL, pColor);
          break;
          case VOCAB_CM_CONFIGURED:
              pColor=&color_grey;
              strcat(frame_title," (CONFIGURED)");
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
  }
  for (k = 0; k < NUMBER_OF_JOINTS; k++)
  {
      if (currentPart->first_time==false && interactionModes[k] == interactionModesOld[k]) continue;
      interactionModesOld[k]=interactionModes[k];
      switch (interactionModes[k])
      {
           case VOCAB_IM_STIFF:
               gtk_widget_modify_base ((GtkWidget*)inEntry[k], GTK_STATE_NORMAL, &color_green);
           break;
           case VOCAB_IM_COMPLIANT:
               gtk_widget_modify_base ((GtkWidget*)inEntry[k], GTK_STATE_NORMAL, &color_fault_red);
           break;
           default:
           case VOCAB_CM_UNKNOWN:
               gtk_widget_modify_base ((GtkWidget*)inEntry[k], GTK_STATE_NORMAL, &color_white);
           break;
      }
  }

  currentPart->first_time =false;
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
  IPositionDirect  *iDir = currentPart->iDir;
  GtkWidget **sliderAry = currentPart->sliderArray;
  IControlMode     *iCtrl = currentPart->ctrlmode2;
  int mode;

  double val = gtk_range_get_value(range);
  double posit = gtk_range_get_value((GtkRange *) sliderAry[*joint]);
  ipos->setRefSpeed(*joint, val);
  iCtrl->getControlMode(*joint, &mode);
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
  IPidControl      *ipid = currentPart->pid;
  IPositionDirect  *iDir = currentPart->iDir;
  GtkWidget **sliderVel = currentPart->sliderVelArray;

  double val = gtk_range_get_value(range);
  double valVel = gtk_range_get_value((GtkRange *)sliderVel[*joint]);

  IControlMode     *iCtrl = currentPart->ctrlmode2;
  int mode;

  iCtrl->getControlMode(*joint, &mode);

  if (!POS_UPDATE[*joint])
    {
      if( ( mode == VOCAB_CM_POSITION) || (mode == VOCAB_CM_MIXED) )
      {
         ipos->setRefSpeed(*joint, valVel);
         ipos->positionMove(*joint, val);
      }
      else if( ( mode == VOCAB_CM_IMPEDANCE_POS))
      {
         fprintf(stderr, " using old 'impedance_position' mode, this control mode is deprecated!");
         ipos->setRefSpeed(*joint, valVel);
         ipos->positionMove(*joint, val);
      }
      else if ( mode == VOCAB_CM_POSITION_DIRECT)
      {
         if (position_direct_enabled)
         {
             iDir->setPosition(*joint, val);
         }
         else
         {
             fprintf(stderr, "You cannot send direct position commands without using --direct option!");
         }

      }
      else
      {
          fprintf(stderr, "Joint not in position nor positionDirect so cannot send references");
      }
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
  IPositionDirect  *iDir = currentPart->iDir;
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
  IPositionDirect  *iDir = currentPart->iDir;

  //fprintf(stderr, "Slider_unpick has been called %d \n", *joint);
  POS_UPDATE[*joint] = true;
  //partMover *currentPart = currentClassData->partPointer;

  
  return;
}
