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
#include "include/guiPid2.h"
#include "include/guiControl.h"
#include <string.h>

  GtkWidget *frame1;
  GtkWidget *frame2;


partMover::partMover(GtkWidget *vbox_d, PolyDriver *partDd_d, PolyDriver *debugDd_d, char *partName, ResourceFinder *fnd, bool speed_view_ena, bool enable_calib_all)
{
  first_time=true;
  speed_view_enable=speed_view_ena;

  finder = fnd;

  if (!finder->isNull())
    fprintf(stderr, "Setting a valid finder \n");

  partLabel = partName;
  partDd = partDd_d;
  debugDd = debugDd_d;
  vbox = vbox_d;
  interfaceError = false;

  //default value for unopened interfaces
  pos		= NULL;
  iVel      = NULL;
  iencs		= NULL;
  amp		= NULL;
  pid		= NULL;
  opl		= NULL;
  trq		= NULL;
  imp		= NULL;
  idbg		= NULL;
  lim		= NULL;
  cal		= NULL;
  ctrlmode	= NULL;

  fprintf(stderr, "Opening interfaces...");
  bool ok=true;
  bool ok2=true;
  ok  = partDd->view(pid);
  if (!ok)
    fprintf(stderr, "...pid was not ok...");
  ok &= partDd->view(amp);
  if (!ok)
    fprintf(stderr, "...amp was not ok...");
  ok &= partDd->view(pos);
  if (!ok)
    fprintf(stderr, "...pos was not ok...");
  ok &= partDd->view(iVel);
  if (!ok)
    fprintf(stderr, "...vel was not ok...");
  ok &= partDd->view(lim);
  if (!ok)
    fprintf(stderr, "...lim was not ok...");
  ok &= partDd->view(iencs);
  if (!ok)
    fprintf(stderr, "...enc was not ok...");
  ok &= partDd->view(cal);
  if (!ok)
    fprintf(stderr, "...cal was not ok.\n");
  ok &= partDd->view(trq);
  if (!ok)
    fprintf(stderr, "...trq was not ok.\n");
  ok  = partDd->view(opl);
  if (!ok)
    fprintf(stderr, "...opl was not ok...");
  ok &= partDd->view(imp);
  if (!ok)
    fprintf(stderr, "...imp was not ok.\n");
  ok &= partDd->view(ctrlmode);
  if (!ok)
	fprintf(stderr, "...ctrlmode was not ok.\n");
  
  //this interface is not mandatory
  if (debugDd)
  {
	  ok2 &= debugDd->view(idbg);
	  if (!ok2)
		fprintf(stderr, "...dbg was not ok.\n");
  }

  if (!partDd->isValid()) {
    fprintf(stderr, "Device driver was not valid! \n");
    dialog_severe_error(GTK_MESSAGE_ERROR,(char *) "Device not available.", (char *) "Check available devices", true);
    interfaceError = true;
  }
  else if (!ok) {
    fprintf(stderr, "Error while acquiring interfaces \n");
    dialog_severe_error(GTK_MESSAGE_ERROR,(char *) "Problems acquiring interfaces", (char *) "Check if interface is running", true);
    interfaceError = true;
  }

  COPY_STORED_POS=0;
  COPY_STORED_VEL=0;
  COPY_SEQUENCE=0;
  COPY_TIMING=0;

  if (interfaceError == false)
    {
      fprintf(stderr, "Allocating memory \n");
      STORED_POS   = new double* [NUMBER_OF_STORED];
      STORED_VEL   = new double* [NUMBER_OF_STORED];
      CURRENT_POS_UPDATE = new bool [NUMBER_OF_STORED];

      int j,k;
      for (j = 0; j < NUMBER_OF_STORED; j++)
	{
	  CURRENT_POS_UPDATE[j] = true;
	  STORED_POS[j] = new double [MAX_NUMBER_OF_JOINTS];
	  STORED_VEL[j] = new double [MAX_NUMBER_OF_JOINTS];
	}

      SEQUENCE     = new int [NUMBER_OF_STORED];
      INV_SEQUENCE = new int [NUMBER_OF_STORED];
      TIMING = new double    [NUMBER_OF_STORED];
      index  = new int [MAX_NUMBER_OF_JOINTS];
      SEQUENCE_ITERATOR = new int [0];
      timeout_seqeunce_id = new guint [0];
      entry_id = new guint [0];
      *entry_id = -1;
      timeout_seqeunce_rate = new guint32 [0];

      sliderArray    = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
      sliderVelArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
      currPosArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  currTrqArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  currSpeedArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  currPosArrayLbl = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  currTrqArrayLbl = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  currSpeedArrayLbl = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
      inPosArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  frameColorBack = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  frame_slider1 = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  frame_slider2 = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
	  framesArray = new GtkWidget* [MAX_NUMBER_OF_JOINTS];


      for (j = 0; j < NUMBER_OF_STORED; j++)
	{
	  SEQUENCE[j] = -1;
	  TIMING[j] = -0.1;
	}

      GtkWidget *top_hbox 		 = NULL;
      GtkWidget *bottom_hbox		 = NULL;
      GtkWidget *panel_hbox		 = NULL;

      GtkWidget *inv1 			 = NULL;
      GtkWidget *invArray[MAX_NUMBER_OF_JOINTS];

      GtkWidget *homeArray[MAX_NUMBER_OF_JOINTS];
      GtkWidget *disableArray[MAX_NUMBER_OF_JOINTS];
      GtkWidget *calibrateArray[MAX_NUMBER_OF_JOINTS];
      GtkWidget *enableArray[MAX_NUMBER_OF_JOINTS];
	  GtkWidget *pidArray[MAX_NUMBER_OF_JOINTS];


      GtkWidget *sw				 = NULL;

      //creation of the top_hbox
      top_hbox = gtk_hbox_new (FALSE, 0);
      gtk_container_set_border_width (GTK_CONTAINER (top_hbox), 10);
      gtk_container_add (GTK_CONTAINER (vbox), top_hbox);
		
      inv1 = gtk_fixed_new ();
      gtk_container_add (GTK_CONTAINER (top_hbox), inv1);
		
      double positions[MAX_NUMBER_OF_JOINTS];
	
	  bool ret=false;
      Time::delay(0.050);
	  do 
	  {
		ret=iencs->getEncoders(positions);
		if (!ret)
		{
		  fprintf(stderr, "%s iencs->getEncoders() failed, retrying...\n", partName);
		  Time::delay(0.050);
		}
	  }
      while (!ret);
	
	  fprintf(stderr, "%s iencs->getEncoders() ok!\n", partName);
      double min = 0;
      double max = 100;
      char buffer[40] = {'i', 'n', 'i', 't'};
		
	  //Here you can set the number of boxes for each row of the GUI
      int numberOfRows = 6;
		
      int NUMBER_OF_JOINTS;
      pos->getAxes(&NUMBER_OF_JOINTS);
		
      int height, width;
      height = 200;
      width = 200;

      for (k = 0; k<MAX_NUMBER_OF_JOINTS; k++)
      {
          sliderVelArray[k]=0;
          sliderArray[k]=0;
          currPosArray[k]=0;
          currTrqArray[k]=0;
          currSpeedArray[k]=0;
          currPosArrayLbl[k]=0;
          currTrqArrayLbl[k]=0;
          currSpeedArrayLbl[k]=0;
          inPosArray[k]=0;
          frameColorBack[k]=0;
          frame_slider1[k]=0;
          frame_slider2[k]=0;
          framesArray[k]=0;
      }

      for (k = 0; k<NUMBER_OF_JOINTS; k++)
	{
	  
	  invArray[k] = gtk_fixed_new ();

	  //init stored
	  for (j = 0; j < NUMBER_OF_STORED; j++)
	    STORED_POS[j][k] = 0.0;

	  //init velocities
	  pos->setRefSpeed(k, ARM_VELOCITY[k]);
			
	  index[k]=k;
	  lim->getLimits(k, &min, &max);
	  j = k/numberOfRows;
			
	  sprintf(buffer, "Joint%d", k);
	  //Objects

	  frame_slider1[k] = gtk_frame_new ("Position:");
	  frame_slider2[k] = gtk_frame_new ("Velocity:");
	  frameColorBack[k] = gtk_event_box_new();

	  //fprintf(stderr, "Initializing sliders %d \n",k);
	  if (min<max)
	    sliderArray[k]	  =  gtk_hscale_new_with_range(min, max, 1);
	  else
	    sliderArray[k]    =  gtk_hscale_new_with_range(1, 2, 1);
	  currPosArray[k]   =  gtk_entry_new();
	  currTrqArray[k]   =  gtk_entry_new();
	  currSpeedArray[k]   =  gtk_entry_new();
  	  currPosArrayLbl[k]  =  gtk_label_new("deg");
	  currTrqArrayLbl[k]  =  gtk_label_new("Nm");
	  currSpeedArrayLbl[k]  =  gtk_label_new("deg/s");
	  inPosArray[k]     = gtk_entry_new();

	  sliderVelArray[k] =  gtk_hscale_new_with_range(1, 100, 1);

	  //fprintf(stderr, "Initializing the buttons %d \n", k);
	  disableArray[k]   = gtk_button_new_with_mnemonic ("Idle");
	  homeArray[k]		= gtk_button_new_with_mnemonic ("Home");
	  calibrateArray[k] = gtk_button_new_with_mnemonic ("Calib");
	  enableArray[k]	= gtk_button_new_with_mnemonic ("Run!");
	  pidArray[k]    	= gtk_button_new_with_mnemonic ("PID");
	  //fprintf(stderr, "Initializing frames %d \n", k);
	  framesArray[k]	= gtk_frame_new (buffer);

			
	  //Positions
	  //fprintf(stderr, "Positioning buttons %d \n", k);

	  gtk_fixed_put	(GTK_FIXED(inv1), frameColorBack[k],    0+(k%numberOfRows)*width, 0+ j*height);
	
	  gtk_container_add (GTK_CONTAINER (frameColorBack[k]), invArray[k]);

	  int sliderOffsetY = 16;
	  int sliderOffsetX = 10;
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), frame_slider1[k],  60+sliderOffsetX, 10+sliderOffsetY    );
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), sliderArray[k],    65+sliderOffsetX, 20+sliderOffsetY   );
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), sliderVelArray[k], 65+sliderOffsetX, 20+50+sliderOffsetY );
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), currPosArray[k],   95+sliderOffsetX, 15+100+sliderOffsetY);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), currTrqArray[k],   95+sliderOffsetX, 35+100+sliderOffsetY);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), currPosArrayLbl[k],   145+sliderOffsetX, 15+103+sliderOffsetY);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), currTrqArrayLbl[k],   145+sliderOffsetX, 35+103+sliderOffsetY);

	  gtk_fixed_put	(GTK_FIXED(invArray[k]), inPosArray[k],     65+sliderOffsetX, 15+100+sliderOffsetY);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), frame_slider2[k],  60+sliderOffsetX, 60+sliderOffsetY    );
	  if (speed_view_enable)
	  {
		  gtk_fixed_put	(GTK_FIXED(invArray[k]), currSpeedArray[k], 95+sliderOffsetX, 55+100+sliderOffsetY);
		  gtk_fixed_put	(GTK_FIXED(invArray[k]), currSpeedArrayLbl[k],   145+sliderOffsetX, 55+103+sliderOffsetY);
	  }

	  int buttonDist= 26;
	  int buttonOffset = 26;
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), homeArray[k],      6, buttonOffset);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), disableArray[k],   6, buttonDist + buttonOffset) ;
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), calibrateArray[k], 6, 2*buttonDist + buttonOffset);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), enableArray[k],    6, 3*buttonDist + buttonOffset);
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), pidArray[k],       6, int(4.5*buttonDist + buttonOffset));
	  gtk_fixed_put	(GTK_FIXED(invArray[k]), framesArray[k],    0,  0);
		
	  //Dimensions
	  //fprintf(stderr, "Dimensioning buttons %d \n", k);
	  gtk_widget_set_size_request 	(frame_slider1[k], 110, 50);
	  gtk_widget_set_size_request   (frameColorBack[k], width, height);
	  gtk_widget_set_size_request 	(sliderArray[k], 90, 40);
	  gtk_widget_set_size_request 	(currPosArray[k], 50, 20);
	  gtk_widget_set_size_request 	(currTrqArray[k], 50, 20);
	  gtk_widget_set_size_request 	(currSpeedArray[k], 50, 20);
	  gtk_widget_set_size_request 	(inPosArray[k], 20, 20);
	  gtk_widget_set_size_request 	(homeArray[k], 50, 25);
	  gtk_widget_set_size_request 	(disableArray[k], 50, 25);
	  gtk_widget_set_size_request 	(calibrateArray[k], 50, 25);
	  gtk_widget_set_size_request 	(enableArray[k], 50, 25);
  	  gtk_widget_set_size_request 	(pidArray[k], 50, 25);
	  gtk_widget_set_size_request 	(frame_slider2[k], 110, 50);
	  gtk_widget_set_size_request 	(sliderVelArray[k], 90, 40);
	  gtk_widget_set_size_request 	(framesArray[k], width, height);
			
	  //Positions commands update
	  //fprintf(stderr, "Assinging callback %d \n", k);
	  gtk_range_set_update_policy 	((GtkRange *) (sliderArray[k]), GTK_UPDATE_DISCONTINUOUS);
	  gtk_range_set_value 			((GtkRange *) (sliderArray[k]),  positions[k]);
	  //g_signal_connect (sliderArray[k], "value-changed", G_CALLBACK(slider_release), index+k );

	  gtkClassData *myClassData = new gtkClassData;
	  myClassData->indexPointer = index+k;
	  myClassData->partPointer = this;
	  g_signal_connect (sliderArray[k], "value-changed", G_CALLBACK(this->slider_release), myClassData);
	  g_signal_connect (sliderArray[k], "focus-in-event", G_CALLBACK(this->slider_pick), myClassData);
	  g_signal_connect (sliderArray[k], "focus-out-event", G_CALLBACK(this->slider_unpick), myClassData);

	  //Positions dispay update
	  gtkClassData *myClassData1 = new gtkClassData;
	  myClassData1->indexPointer = index+k;
	  myClassData1->partPointer = this;

	  gtk_editable_set_editable ((GtkEditable*) currPosArray[k], FALSE);
	  gtk_editable_set_editable ((GtkEditable*) currTrqArray[k], FALSE);
	  gtk_editable_set_editable ((GtkEditable*) currSpeedArray[k], FALSE);
	  gtk_editable_set_editable ((GtkEditable*) inPosArray[k], FALSE);

	  //Velocity commands update
	  gtk_range_set_update_policy 	((GtkRange *) (sliderVelArray[k]), GTK_UPDATE_DISCONTINUOUS);
	  gtk_range_set_value 			((GtkRange *) (sliderVelArray[k]), ARM_VELOCITY[k]);
	  g_signal_connect (sliderVelArray[k], "value-changed", G_CALLBACK (sliderVel_release), myClassData1);
	  //Run updatedisable_entry
	  g_signal_connect (homeArray[k], "clicked", G_CALLBACK (home_click), myClassData1);
	  g_signal_connect (disableArray[k], "clicked", G_CALLBACK (dis_click), myClassData1);
	  g_signal_connect (calibrateArray[k], "clicked", G_CALLBACK (calib_click), myClassData1);
      if (enable_calib_all==false) gtk_widget_set_sensitive(calibrateArray[k], false);

	  g_signal_connect (enableArray[k], "clicked", G_CALLBACK (run_click), myClassData1);
	  g_signal_connect (pidArray[k], "clicked", G_CALLBACK (pid_click), myClassData1);		
	  //control mode
	  g_signal_connect(frameColorBack[k], "button-press-event", G_CALLBACK (control_mode_click), myClassData1);

	}

      
      
      gtkClassData *myClassData3 = new gtkClassData;
      myClassData3->indexPointer = index+k;
      myClassData3->partPointer = this;
      //g_signal_connect (treeview, "row-activated", G_CALLBACK (line_click),myClassData3);
      

      frame1 = gtk_frame_new ("Commands:");
      gtk_fixed_put	(GTK_FIXED(inv1), frame1,       (NUMBER_OF_JOINTS%numberOfRows)*width,         (NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request 	(frame1, width, height);
		      
      button1 = NULL;
      button2 = NULL;
      button3 = NULL;
      button4 = NULL;
      button5 = NULL;
      button6 = NULL;
      button7 = NULL;
      button8 = NULL;

	  //Button15 in the panel
      GtkWidget *button15 = gtk_button_new_with_mnemonic ("IdleAll");
      //gtk_container_add (GTK_CONTAINER (panel_hbox), button0);
      g_signal_connect (button15, "clicked", G_CALLBACK (idle_all), this);
      gtk_fixed_put (GTK_FIXED (inv1), button15, 25+(NUMBER_OF_JOINTS%numberOfRows)*width,        160+(NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request 	(button15, 150, 30);

      //Button 14 in the panel
      GtkWidget *button14 = gtk_button_new_with_mnemonic ("Open sequence tab");
      //gtk_container_add (GTK_CONTAINER (panel_hbox), button1);
      g_signal_connect (button14, "clicked", G_CALLBACK (table_open), myClassData3);
      gtk_fixed_put (GTK_FIXED (inv1), button14, 25+(NUMBER_OF_JOINTS%numberOfRows)*width,         20+(NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request     (button14, 150, 30);

      //Button13 in the panel
      GtkWidget *button13 = gtk_button_new_with_mnemonic ("RunAll");
      //gtk_container_add (GTK_CONTAINER (panel_hbox), button0);
      g_signal_connect (button13, "clicked", G_CALLBACK (run_all), this);
      gtk_fixed_put (GTK_FIXED (inv1), button13, 25+(NUMBER_OF_JOINTS%numberOfRows)*width,         50+(NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request 	(button13, 150, 30);

      //Button11 in the panel
      GtkWidget *button11 = gtk_button_new_with_mnemonic ("CalibAll");
      //gtk_container_add (GTK_CONTAINER (panel_hbox), button0);
      if (enable_calib_all==false) gtk_widget_set_sensitive(button11, false);
      g_signal_connect (button11, "clicked", G_CALLBACK (calib_all), this);
      gtk_fixed_put (GTK_FIXED (inv1), button11, 25+(NUMBER_OF_JOINTS%numberOfRows)*width,         80+(NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request 	(button11, 150, 30);

      //Button12 in the panel
      GtkWidget *button12 = gtk_button_new_with_mnemonic ("HomeAll");
      //gtk_container_add (GTK_CONTAINER (panel_hbox), button0);
      g_signal_connect (button12, "clicked", G_CALLBACK (home_all), this);
      gtk_fixed_put (GTK_FIXED (inv1), button12, 25+(NUMBER_OF_JOINTS%numberOfRows)*width,         110+(NUMBER_OF_JOINTS/numberOfRows)*height);
      gtk_widget_set_size_request 	(button12, 150, 30);
     
    }
}

partMover::~partMover()
{
  fprintf(stderr, "destroying part mover \n");
}

void partMover::releaseDriver()
{
  fprintf(stderr, "closing driver...");
  partDd->close();
  fprintf(stderr, "driver was closed!\n");
}

void partMover::pid_click(GtkButton *button, gtkClassData* currentClassData)
{
	guiPid2::guiPid2(button, currentClassData);
}

void partMover::control_mode_click(GtkButton *button, GdkEventButton *event, gtkClassData* currentClassData)
{
//view_onButtonPressed (GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)
	{
		   //   g_print ("Single right click\n");
		   guiControl::guiControl(button, currentClassData);
	}
}
