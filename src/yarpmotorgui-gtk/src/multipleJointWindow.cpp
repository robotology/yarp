#include "include/robotMotorGui.h"
#include "include/partMover.h"
#include <string.h>


/*
 * Performs coordinated movements
 * in a given amount of time
 */

void partMover::fixed_time_move(const double *cmdPositions, double cmdTime, partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  ITorqueControl *itrq= currentPart->trq;

  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  int NUM_JOINTS;
  ipos->getAxes(&NUM_JOINTS);
  double *cmdVelocities = new double[NUM_JOINTS];
  double *startPositions = new double[NUM_JOINTS];


  while (!iiencs->getEncoders(startPositions))
    Time::delay(0.001);
    //fprintf(stderr, "getEncoders is returning false\n");
    //fprintf(stderr, "Getting the following values for the encoders");
  //for(int k=0; k<NUM_JOINTS; k++)
  //  fprintf(stderr, "%.1f ", startPositions[k]);
  //fprintf(stderr, "\n");  

  int k;
  for(k=0; k<NUM_JOINTS; k++)
    {
      cmdVelocities[k] = 0;
		
      if (fabs(startPositions[k] - cmdPositions[k]) > 0.01)
	cmdVelocities[k] = fabs(startPositions[k] - cmdPositions[k])/cmdTime;
      else
	cmdVelocities[k] = 1.0;
    }

  //fprintf(stderr, "ExplorerThread-> Start pos:\n");
  //for(int j=0; j < NUM_JOINTS; j++)
  //  fprintf(stderr, "%.2lf\t", startPositions[j]);
  //fprintf(stderr, "\n");
  //fprintf(stderr, "ExplorerThread-> Moving arm to:\n");
  //for(int j=0; j < NUM_JOINTS; j++)
  //  fprintf(stderr, "%.2lf\t", cmdPositions[j]);
  //fprintf(stderr, "\n");
  //fprintf(stderr, "ExplorerThread-> with velocity:\n");
  //for(int ii=0; ii < NUM_JOINTS; ii++)
  //  fprintf(stderr, "%.2lf\t", cmdVelocities[ii]);
  //fprintf(stderr, "\n");

  ipos->setRefSpeeds(cmdVelocities);
  ipos->positionMove(cmdPositions);	

  delete cmdVelocities;
  delete startPositions;
  return;
}

/*
 * Save to file the current list of positions
 */
void partMover::save_to_file(char* filenameIn, partMover* currentPart)
{
  char filename[800]; 
  char filenamePart[800];
  FILE* outputFile;

  IPositionControl *ipos = currentPart->pos;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  int j, k;
  char buffer[800];
  int invSequence[NUMBER_OF_STORED];
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  fprintf(stderr, "Start saving file\n");
  strcpy(filename, filenameIn);
  strcat(filename, ".pos");
  strcpy(filenamePart, strcat(filename, currentPart->partLabel));
  //fprintf(stderr, "%s", filenamePart);
  outputFile = fopen(filenamePart,"w");
	
  for (j = 0; j < NUMBER_OF_STORED; j++)
    invSequence[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (SEQUENCE_TMP[j]>-1 && (SEQUENCE_TMP[j]<NUMBER_OF_STORED))
	invSequence[SEQUENCE_TMP[j]] = j;
    }

  for (j = 0; j < NUMBER_OF_STORED; j++)
    if (invSequence[j] != -1)
      {
	sprintf(buffer, "[POSITION%d] \n", j);

	fprintf(outputFile, "%s", buffer);
	//Sequence positions
	fprintf(outputFile, "jointPositions ");
	for (k = 0; k < NUMBER_OF_JOINTS; k++)
	  fprintf(outputFile, "%.2f ", STORED_POS_TMP[ invSequence[j] ][k]);
	fprintf(outputFile, "\n");
	//Sequence Velocities
	fprintf(outputFile, "jointVelocities ");
	for (k = 0; k < NUMBER_OF_JOINTS; k++)
	  fprintf(outputFile, "%.2f ", STORED_VEL_TMP[ invSequence[j] ][k]);
	fprintf(outputFile, "\n");
	//Timing
	fprintf(outputFile, "timing ");
	fprintf(outputFile, "%.2f ", TIMING_TMP[invSequence[j]]);
	fprintf(outputFile, "\n");
      }
    else
      break;
  fclose(outputFile);
  fprintf(stderr, "File saved and closed\n");
}

/*
 * Load from file the current list of positions
 */
void partMover::load_from_file(char* filenameIn, partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  GtkWidget *tree_view = currentPart->treeview;

  int j, k, extensionLength, filenameLength;
  char buffer[800];
  char filenameExtension[800];

  Property p;
  bool fileExists = p.fromConfigFile(filenameIn);	
  strcpy(filenameExtension, ".pos");
  strcat(filenameExtension, currentPart->partLabel);
  extensionLength = strlen(filenameExtension);
  filenameLength = strlen(filenameIn);
  if ((filenameLength - extensionLength) > 0)
    fileExists &= (strcmp(filenameIn + 
			  (sizeof(char))*(filenameLength - extensionLength), 
			  filenameExtension) == 0 );
  else
    fileExists=false;
	
  if (fileExists)
    {
      int NUMBER_OF_JOINTS;
      ipos->getAxes(&NUMBER_OF_JOINTS);
		
      for (j = 0; j < NUMBER_OF_STORED; j++)
	{
	  sprintf(buffer, "POSITION%d", j);
	  Bottle& xtmp = p.findGroup(buffer).findGroup("jointPositions");
	  if (xtmp.size() == NUMBER_OF_JOINTS+1 && j < NUMBER_OF_STORED - 1)
	    {
	      for (k = 0; k < NUMBER_OF_JOINTS; k++)
		STORED_POS_TMP[j][k] = xtmp.get(k+1).asDouble();
				
	      xtmp = p.findGroup(buffer).findGroup("jointVelocities");
	      for (k = 0; k < NUMBER_OF_JOINTS; k++)
		STORED_VEL_TMP[j][k] = xtmp.get(k+1).asDouble();
				
				
	      xtmp = p.findGroup(buffer).findGroup("timing");
	      TIMING_TMP[j] = xtmp.get(1).asDouble();
	      SEQUENCE_TMP[j] = j;
	    }
	  else
	    {
	      for (k = 0; k < NUMBER_OF_JOINTS; k++)
		STORED_POS_TMP[j][k] = 0.0;
	      SEQUENCE_TMP[j] = -1;
	      if(j==0)
		{
		  dialog_message(GTK_MESSAGE_ERROR,
				 (char *) "Couldn't load a valid position file.", 
				 (char *) "Check the format of the supplied file.", true);
		  return;
		}
	      if(j == NUMBER_OF_STORED - 1 && xtmp.size() == NUMBER_OF_JOINTS+1)
		{
		  dialog_message(GTK_MESSAGE_ERROR,
			      (char *) "Truncating the current sequence which is too long. You need to recompile with a greater value of NUMBER_OF_STORED", 
			      (char *) "Unfortunately maximum sequence length is not set at runtime", true);
		}
	    }
	}
      if(GTK_IS_TREE_VIEW (tree_view))	
	{
	  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_position_list_model(currentPart));
	  gtk_widget_draw(GTK_WIDGET(tree_view), NULL);
	}
    }
  else
    dialog_message(GTK_MESSAGE_ERROR,
				(char *) "Wrong format (check estensions) of the file associated with:", 
				currentPart->partLabel, true);
  return;
}

/*
 * Enable all PID and refresh position sliders
 */

void partMover::run_all(GtkButton *button, partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  GtkWidget **sliderAry = currentPart->sliderArray;
	 
  double posJoint;
  int joint;
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
  
  for (joint=0; joint < NUMBER_OF_JOINTS; joint++)
  {
    iiencs->getEncoder(joint, &posJoint);
    iamp->enableAmp(joint);
    ipid->enablePid(joint);
    gtk_range_set_value ((GtkRange *) (sliderAry[joint]), posJoint);
  }
  return;
}

void partMover::home_all(GtkButton *button, partMover* currentPart)
{
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
      bool ok = true;
      Bottle xtmp, ytmp;
      xtmp = fnd->findGroup(buffer2).findGroup("PositionZero");
      ok = ok && (xtmp.size() == NUMBER_OF_JOINTS+1);
      ytmp = fnd->findGroup(buffer2).findGroup("VelocityZero");
      ok = ok && (ytmp.size() == NUMBER_OF_JOINTS+1);
      if(ok)
	{
	  for (int joint = 0; joint < NUMBER_OF_JOINTS; joint++)
	    {
	      double positionZero = xtmp.get(joint+1).asDouble();
	      //fprintf(stderr, "%f ", positionZero);

	      double velocityZero = ytmp.get(joint+1).asDouble();
	      //fprintf(stderr, "%f ", velocityZero);

	      ipos->setRefSpeed(joint, velocityZero);
	      ipos->positionMove(joint, positionZero);
	    }
	}
      else
	dialog_message(GTK_MESSAGE_ERROR,(char *) "Check the number of entries in the group",  buffer2, true);
    }
  else
    {
      //		currentPart->dialog_message(GTK_MESSAGE_ERROR,"No calib file found", strcat("Define a suitable ", strcat(currentPart->partLabel, "Calib")), true);        
      dialog_message(GTK_MESSAGE_ERROR,(char *) "No zero group found in the supplied file. Define a suitable",  buffer2, true);   
    }
  return;
}

/*
 * Enable all PID and refresh position sliders
 */

void partMover::calib_all(GtkButton *button, partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IControlCalibration2 *ical = currentPart->cal;
	 
  int joint;
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
      Bottle p1 = fnd->findGroup(buffer2).findGroup("Calibration1");
      ok = ok && (p1.size() == NUMBER_OF_JOINTS+1);
      Bottle p2 = fnd->findGroup(buffer2).findGroup("Calibration2");
      ok = ok && (p2.size() == NUMBER_OF_JOINTS+1);
      Bottle p3 = fnd->findGroup(buffer2).findGroup("Calibration3");
      ok = ok && (p3.size() == NUMBER_OF_JOINTS+1);
      Bottle ty = fnd->findGroup(buffer2).findGroup("CalibrationType");
      ok = ok && (ty.size() == NUMBER_OF_JOINTS+1);
      if (ok)
	{
	  for (joint=0; joint < NUMBER_OF_JOINTS; joint++)
	    {

	      double param1 = p1.get(joint+1).asDouble();
	      //fprintf(stderr, "%f ", param1);


	      double param2 = p2.get(joint+1).asDouble();
	      //fprintf(stderr, "%f ", param2);


	      double param3 = p3.get(joint+1).asDouble();
	      //fprintf(stderr, "%f ", param3);

	      unsigned char type = (unsigned char) ty.get(joint+1).asDouble();
	      //fprintf(stderr, "%d ", type);

	      ical->calibrate2(joint, type, param1, param2, param3);
	    }
	}
      else
	dialog_message(GTK_MESSAGE_ERROR,(char *) "Check number of entries in the file",  buffer1, true);
    }
  else
    {
      dialog_message(GTK_MESSAGE_ERROR,(char *) "No calib file found. Define a suitable file called:",  buffer1, true);   
    }
  return;

}


/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */

void partMover::sequence_click(GtkButton *button, partMover* currentPart)
{

  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;

  int j;
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
	
  int invSequence[NUMBER_OF_STORED];

  for (j = 0; j < NUMBER_OF_STORED; j++)
    invSequence[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (SEQUENCE_TMP[j]>-1 && (SEQUENCE_TMP[j]<NUMBER_OF_STORED))
	invSequence[SEQUENCE_TMP[j]] = j;
    }
  for (j = 0; j < NUMBER_OF_STORED; j++)
    if (invSequence[j]!=-1)
      {
	if (TIMING_TMP[invSequence[j]] > 0)
	  {
	    ipos->setRefSpeeds(STORED_VEL_TMP[invSequence[j]]);
	    ipos->positionMove(STORED_POS_TMP[invSequence[j]]);
	    for (int k =0; k < NUMBER_OF_JOINTS; k++)
	      {
		gtk_range_set_value ((GtkRange *) (sliderAry[k]),    STORED_POS_TMP[invSequence[j]][k]);
		gtk_range_set_value ((GtkRange *) (sliderVelAry[k]), STORED_VEL_TMP[invSequence[j]][k]);
	      }
	    Time::delay(TIMING_TMP[invSequence[j]]);
	  }
      }
    else
      break;
  return;
}


/*
 * If go is clicked retrieve the current 
 * selection and move to the given position.
 * Finally update sliders
 */

void partMover::go_click(GtkButton *button, partMover *currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;

  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
		
  //get the current row index
  int i = get_index_selection(currentPart);
  if (i != -1)
    {
      if (TIMING_TMP[i]>0)
	{
	  ipos->setRefSpeeds(STORED_VEL_TMP[i]);
	  ipos->positionMove(STORED_POS_TMP[i]);
	  for (int k =0; k < NUMBER_OF_JOINTS; k++)
	    {
	      gtk_range_set_value ((GtkRange *) (sliderAry[k]),  STORED_POS_TMP[i][k]);
	      gtk_range_set_value ((GtkRange *) (sliderVelAry[k]), STORED_VEL_TMP[i][k]);
	    }
	}
    }
		
  return;
}



/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */

void partMover::sequence_time(GtkButton *button, partMover* currentPart)
{

  IPositionControl *ipos = currentPart->pos;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  int invSequence[NUMBER_OF_STORED];
  int NUMBER_OF_JOINTS;
  int j;
	
  ipos->getAxes(&NUMBER_OF_JOINTS);	


  for (j = 0; j < NUMBER_OF_STORED; j++)
    invSequence[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (SEQUENCE_TMP[j]>-1 && (SEQUENCE_TMP[j]<NUMBER_OF_STORED))
	invSequence[SEQUENCE_TMP[j]] = j;
    }
  for (j = 0; j < NUMBER_OF_STORED; j++)
    if (invSequence[j]!=-1)
      {
	if (TIMING_TMP[invSequence[j]] > 0)
	  { 	    
	    fixed_time_move(STORED_POS_TMP[invSequence[j]],
			    TIMING_TMP[invSequence[j]],
			    currentPart);
	    Time::delay(TIMING_TMP[invSequence[j]]);
	  }
      }
    else
      break;
  return;
}

/*
 * Saves the current sequence in the 
 * correct order.
 */

//*********************************************************************************
void partMover::sequence_save(GtkButton *button,  partMover* currentPart)
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Save File",
					(GtkWindow*) window,
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
	
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filenameIn; 
	  	  
      filenameIn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      save_to_file (filenameIn,currentPart);
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;
}

//*********************************************************************************

/*
 * Load a stored sequence
 */
void partMover::sequence_load(GtkFileChooser *button, partMover *currentPart )
{
  gchar* filePath = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(button));
  if (filePath == NULL) return;
  load_from_file(filePath, currentPart);
  return;
}

/*
 * If sequence cycle is pressed a new thread 
 * is started. The thread will use the function
 * sequence_iterator to decide the next movement.
 * The thread rate (timeout_seqeunce_rate) corresponds
 * to the minimum delay between movements. The function
 * sequence_iterator  takes care of waiting the remaining
 * amount of time.
 */

void partMover::sequence_cycle(GtkButton *button,partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  int *INV_SEQUENCE_TMP = currentPart->INV_SEQUENCE;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  GtkWidget *tree_view = currentPart->treeview;
  guint32* timeout_seqeunce_rate_tmp = currentPart->timeout_seqeunce_rate;
  guint* timeout_seqeunce_id_tmp = currentPart->timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR_TMP = currentPart->SEQUENCE_ITERATOR;
	
  int j, k;
  *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[0]*1000);
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  for (j = 0; j < NUMBER_OF_STORED; j++)
    INV_SEQUENCE_TMP[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (SEQUENCE_TMP[j]>-1 && (SEQUENCE_TMP[j]<NUMBER_OF_STORED))
	{
	  INV_SEQUENCE_TMP[SEQUENCE_TMP[j]] = j;
	}
    }

  //if possible execute the first movement
  //SEQUENCE_ITERATOR = 0;
  if (INV_SEQUENCE_TMP[0]!=-1 && TIMING_TMP[0] >0 )
    {
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction)sequence_iterator, currentPart);
      ipos->setRefSpeeds(STORED_VEL_TMP[INV_SEQUENCE_TMP[0]]);
      ipos->positionMove(STORED_POS_TMP[INV_SEQUENCE_TMP[0]]);
      for (k =0; k < NUMBER_OF_JOINTS; k++)
	{
	  gtk_range_set_value ((GtkRange *) (sliderAry[k]),    STORED_POS_TMP[INV_SEQUENCE_TMP[0]][k]);
	  gtk_range_set_value ((GtkRange *) (sliderVelAry[k]), STORED_VEL_TMP[INV_SEQUENCE_TMP[0]][k]);
	}
      //point the SEQUENCE ITERATOR to the next movement
      *SEQUENCE_ITERATOR_TMP = 1;

      //deactivate all buttons
      for (k =0; k < NUMBER_OF_JOINTS; k++)
	{
	  gtk_widget_set_sensitive(sliderVelAry[k], false);
	  gtk_widget_set_sensitive(sliderAry[k], false);
	}

      //fprintf(stderr, "Disabling bottons\n");
      if (currentPart->button1 != NULL)
	gtk_widget_set_sensitive(currentPart->button1, false);
      if (currentPart->button2 != NULL)
	gtk_widget_set_sensitive(currentPart->button2, false);
      if (currentPart->button3 != NULL)
	gtk_widget_set_sensitive(currentPart->button3, false);
      if (currentPart->button4 != NULL)
	gtk_widget_set_sensitive(currentPart->button4, false);
      if (currentPart->button5 != NULL)
	gtk_widget_set_sensitive(currentPart->button5, false);
      if (currentPart->button7 != NULL)
	gtk_widget_set_sensitive(currentPart->button7, false);
      if (currentPart->button8 != NULL)
	gtk_widget_set_sensitive(currentPart->button8, false);

    }
  return;
}

/*
 * If sequence cycle (time) is pressed a new thread 
 * is started. The thread will use the function
 * sequence_iterator to decide the next movement.
 * The thread rate (timeout_seqeunce_rate) corresponds
 * to the minimum delay between movements. The function
 * sequence_iterator  takes care of waiting the remaining
 * amount of time. Movements are coordinated so to last
 * the given amount of time
 */

void partMover::sequence_cycle_time(GtkButton *button,partMover* currentPart)
{
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
  double **STORED_VEL_TMP = currentPart->STORED_VEL;
  int *INV_SEQUENCE_TMP = currentPart->INV_SEQUENCE;
  GtkWidget **sliderAry = currentPart->sliderArray;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  GtkWidget *tree_view = currentPart->treeview;
  guint32* timeout_seqeunce_rate_tmp = currentPart->timeout_seqeunce_rate;
  guint* timeout_seqeunce_id_tmp = currentPart->timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR_TMP = currentPart->SEQUENCE_ITERATOR;
	
  int j, k;
  *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[0]*1000);
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  //fprintf(stderr, "Getting the number of joitns %d\n", NUMBER_OF_JOINTS);

  for (j = 0; j < NUMBER_OF_STORED; j++)
    INV_SEQUENCE_TMP[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (SEQUENCE_TMP[j]>-1 && (SEQUENCE_TMP[j]<NUMBER_OF_STORED))
	{
	  INV_SEQUENCE_TMP[SEQUENCE_TMP[j]] = j;
	}
    }

  //if possible execute the first movement
  //SEQUENCE_ITERATOR = 0;
  if (INV_SEQUENCE_TMP[0]!=-1 && TIMING_TMP[0] >0 )
    {
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction)sequence_iterator_time, currentPart);
      fixed_time_move(STORED_POS_TMP[INV_SEQUENCE_TMP[0]],
			    TIMING_TMP[0],
			    currentPart);
      
      //point the SEQUENCE ITERATOR to the next movement
      *SEQUENCE_ITERATOR_TMP = 1;

      //deactivate all buttons
      for (k =0; k < NUMBER_OF_JOINTS; k++)
	{
	  gtk_widget_set_sensitive(sliderVelAry[k], false);
	  gtk_widget_set_sensitive(sliderAry[k], false);
	}

      //fprintf(stderr, "Disabling bottons\n");
      if (currentPart->button1 != NULL)
	gtk_widget_set_sensitive(currentPart->button1, false);
      if (currentPart->button2 != NULL)
	gtk_widget_set_sensitive(currentPart->button2, false);
      if (currentPart->button3 != NULL)
	gtk_widget_set_sensitive(currentPart->button3, false);
      if (currentPart->button4 != NULL)
	gtk_widget_set_sensitive(currentPart->button4, false);
      if (currentPart->button5 != NULL)
	gtk_widget_set_sensitive(currentPart->button5, false);
      if (currentPart->button7 != NULL)
	gtk_widget_set_sensitive(currentPart->button7, false);
      if (currentPart->button8 != NULL)
	gtk_widget_set_sensitive(currentPart->button8, false);

    }
  //fprintf(stderr, "Iterator was initialized \n");
  return;
}


/* Decide the next movement
 * and waits enough time so that
 * the total amount of delay corresponds
 * to the desired 
 */

bool partMover::sequence_iterator(partMover* currP)
{
  //fprintf(stderr, "calling sequence iterator \n");
  IPositionControl *ipos = currP->pos;
  IEncoders *iiencs = currP->iencs;
  IAmplifierControl *iamp = currP->amp;
  IPidControl *ipid = currP->pid;
  int *SEQUENCE_TMP = currP->SEQUENCE;
  double *TIMING_TMP = currP->TIMING;
  double **STORED_POS_TMP = currP->STORED_POS;
  double **STORED_VEL_TMP = currP->STORED_VEL;
  int *INV_SEQUENCE_TMP = currP->INV_SEQUENCE;
  GtkWidget **sliderAry = currP->sliderArray;
  GtkWidget **sliderVelAry = currP->sliderVelArray;
  GtkWidget *tree_view = currP->treeview;
  guint32* timeout_seqeunce_rate_tmp = currP->timeout_seqeunce_rate;
  guint* timeout_seqeunce_id_tmp = currP->timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR_TMP = currP->SEQUENCE_ITERATOR;

  int j = (*SEQUENCE_ITERATOR_TMP);
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  if (INV_SEQUENCE_TMP[j]!=-1)
    {
      ipos->setRefSpeeds(STORED_VEL_TMP[INV_SEQUENCE_TMP[j]]);
      ipos->positionMove(STORED_POS_TMP[INV_SEQUENCE_TMP[j]]);
      for (int k =0; k < NUMBER_OF_JOINTS; k++)
	{
	  gtk_range_set_value ((GtkRange *) (sliderAry[k]),    STORED_POS_TMP[INV_SEQUENCE_TMP[j]][k]);
	  gtk_range_set_value ((GtkRange *) (sliderVelAry[k]), STORED_VEL_TMP[INV_SEQUENCE_TMP[j]][k]);
	}
      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) sequence_iterator, currP);
    }
  else
    {
      //restart the sequence if finished
      *SEQUENCE_ITERATOR_TMP = 0;
      j = 0;
      ipos->setRefSpeeds(STORED_VEL_TMP[INV_SEQUENCE_TMP[j]]);
      ipos->positionMove(STORED_POS_TMP[INV_SEQUENCE_TMP[j]]);
      for (int k =0; k < NUMBER_OF_JOINTS; k++)
	{
	  gtk_range_set_value ((GtkRange *) (sliderAry[k]),    STORED_POS_TMP[INV_SEQUENCE_TMP[j]][k]);
	  gtk_range_set_value ((GtkRange *) (sliderVelAry[k]), STORED_VEL_TMP[INV_SEQUENCE_TMP[j]][k]);
	}
      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) sequence_iterator, currP);
    }

  return false;
}


/* Decide the next movement
 * and waits enough time so that
 * the total amount of delay corresponds
 * to the desired 
 */

bool partMover::sequence_iterator_time(partMover* currP)
{
  //fprintf(stderr, "calling sequence iterator time \n");
  IPositionControl *ipos = currP->pos;
  IEncoders *iiencs = currP->iencs;
  IAmplifierControl *iamp = currP->amp;
  IPidControl *ipid = currP->pid;
  int *SEQUENCE_TMP = currP->SEQUENCE;
  double *TIMING_TMP = currP->TIMING;
  double **STORED_POS_TMP = currP->STORED_POS;
  double **STORED_VEL_TMP = currP->STORED_VEL;
  int *INV_SEQUENCE_TMP = currP->INV_SEQUENCE;
  GtkWidget **sliderAry = currP->sliderArray;
  GtkWidget **sliderVelAry = currP->sliderVelArray;
  GtkWidget *tree_view = currP->treeview;
  guint32* timeout_seqeunce_rate_tmp = currP->timeout_seqeunce_rate;
  guint* timeout_seqeunce_id_tmp = currP->timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR_TMP = currP->SEQUENCE_ITERATOR;

  int j = (*SEQUENCE_ITERATOR_TMP);
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  if (INV_SEQUENCE_TMP[j]!=-1)
    {
      fixed_time_move(STORED_POS_TMP[INV_SEQUENCE_TMP[j]],
			    TIMING_TMP[j],
			    currP);

      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) sequence_iterator_time, currP);
    }
  else
    {
      //restart the sequence if finished
      *SEQUENCE_ITERATOR_TMP = 0;
      j = 0;
      fixed_time_move(STORED_POS_TMP[INV_SEQUENCE_TMP[j]],
			    TIMING_TMP[j],
			    currP);
      

      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) sequence_iterator_time, currP);
    }

  return false;
}


/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */

void partMover::sequence_stop(GtkButton *button,partMover* currP)
{
  //fprintf(stderr, "calling sequence time stop\n");
  guint* timeout_seqeunce_id_tmp = currP->timeout_seqeunce_id;
  GtkWidget **sliderAry = currP->sliderArray;
  GtkWidget **sliderVelAry = currP->sliderVelArray;
  IPositionControl *ipos = currP->pos;

  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);

  //deactivate all buttons
  int k;
  for (k =0; k < NUMBER_OF_JOINTS; k++)
    {
      gtk_widget_set_sensitive(sliderVelAry[k], true);
      gtk_widget_set_sensitive(sliderAry[k], true);
    }

  //fprintf(stderr, "Enabling bottons...");
  if (currP->button1 != NULL)
    //fprintf(stderr, "botton is sensitive...");
    gtk_widget_set_sensitive(currP->button1, true);
    //fprintf(stderr, "disabled...");     
  if (currP->button2 != NULL)
    gtk_widget_set_sensitive(currP->button2, true);
  if (currP->button3 != NULL)
    gtk_widget_set_sensitive(currP->button3, true);
  if (currP->button4 != NULL)
    gtk_widget_set_sensitive(currP->button4, true);
  if (currP->button5 != NULL)
    gtk_widget_set_sensitive(currP->button5, true);
  if (currP->button6 != NULL)
    gtk_widget_set_sensitive(currP->button6, true);
  if (currP->button7 != NULL)
    gtk_widget_set_sensitive(currP->button7, true);
  if (currP->button8 != NULL)
    gtk_widget_set_sensitive(currP->button8, true);
  //fprintf(stderr, "done!\n");

  gtk_timeout_remove(*timeout_seqeunce_id_tmp);
  return;
}

/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */

//void partMover::sequence_stop_time(GtkButton *button,partMover* currP)
//{
//  //fprintf(stderr, "calling sequence stop time\n");
//  guint* timeout_seqeunce_id_tmp = currP->timeout_seqeunce_id;
//  GtkWidget **sliderAry = currP->sliderArray;
//  GtkWidget **sliderVelAry = currP->sliderVelArray;
//  IPositionControl *ipos = currP->pos;

//  int NUMBER_OF_JOINTS;
//  ipos->getAxes(&NUMBER_OF_JOINTS);

  //deactivate all buttons
//  int k;
//  for (k =0; k < NUMBER_OF_JOINTS; k++)
//    {
//      gtk_widget_set_sensitive(sliderVelAry[k], true);
//      gtk_widget_set_sensitive(sliderAry[k], true);
//    }
//  gtk_widget_set_sensitive(currP->button0, true);
//  gtk_widget_set_sensitive(currP->button1, true);
//  gtk_widget_set_sensitive(currP->button2, true);
//  gtk_widget_set_sensitive(currP->button3, true);
//  gtk_widget_set_sensitive(currP->button4, true);
//  gtk_widget_set_sensitive(currP->button5, true);
//  gtk_widget_set_sensitive(currP->button6, true);
//  gtk_widget_set_sensitive(currP->button7, true);
//  gtk_widget_set_sensitive(currP->button8, true);
//  gtk_widget_set_sensitive(currP->button9, true);

//  gtk_timeout_remove(*timeout_seqeunce_id_tmp);
//  return;
//}
