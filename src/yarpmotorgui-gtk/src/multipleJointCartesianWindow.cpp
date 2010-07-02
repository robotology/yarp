#include "include/robotMotorGui.h"
#include "include/cartesianMover.h"

#include <string.h>

extern GtkTreeModel* refresh_cartesian_list_model(cartesianMover *cm);
extern int get_cartesian_index_selection(cartesianMover *cm);

/*
 * Save to file the current list of positions
 */
void cartesianMover::save_to_file(char* filenameIn, cartesianMover* cm)
{
  char filename[800]; 
  char filenamePart[800];
  FILE* outputFile;

  int j, k;
  char buffer[800];
  int invSequence[NUMBER_OF_STORED];
	
  fprintf(stderr, "Start saving file\n");
  strcpy(filename, filenameIn);
  strcat(filename, ".crt");
  strcpy(filenamePart, strcat(filename, cm->partLabel));
  //fprintf(stderr, "%s", filenamePart);
  outputFile = fopen(filenamePart,"w");
	
  for (j = 0; j < NUMBER_OF_STORED; j++)
    invSequence[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (cm->SEQUENCE[j]>-1 && (cm->SEQUENCE[j]<NUMBER_OF_STORED))
	invSequence[cm->SEQUENCE[j]] = j;
    }

  for (j = 0; j < NUMBER_OF_STORED; j++)
    if (invSequence[j] != -1)
      {
	sprintf(buffer, "[CARTESIAN%d] \n", j);

	fprintf(outputFile, "%s", buffer);
	//Sequence positions
	fprintf(outputFile, "cartesianPositions ");
	for (k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
	  fprintf(outputFile, "%.2f ", cm->STORED_POS[ invSequence[j] ][k]);
	fprintf(outputFile, "\n");
	//Timing
	fprintf(outputFile, "timing ");
	fprintf(outputFile, "%.2f ", cm->TIMING[invSequence[j]]);
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
void cartesianMover::load_from_file(char* filenameIn, cartesianMover* cm)
{
  int j, k, extensionLength, filenameLength;
  char buffer[800];
  char filenameExtension[800];

  Property p;
  bool fileExists = p.fromConfigFile(filenameIn);
  fprintf(stderr, "Loading from file: %s\n", filenameIn);
  strcpy(filenameExtension, ".crt");
  strcat(filenameExtension, cm->partLabel);
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
      for (j = 0; j < NUMBER_OF_STORED; j++)
	{
	  sprintf(buffer, "CARTESIAN%d", j);
	  Bottle& xtmp = p.findGroup(buffer).findGroup("cartesianPositions");
	  if (xtmp.size() == NUMBER_OF_CARTESIAN_COORDINATES+1 && j < NUMBER_OF_STORED - 1)
	    {
	      for (k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
		cm->STORED_POS[j][k] = xtmp.get(k+1).asDouble();
				
	      xtmp = p.findGroup(buffer).findGroup("timing");
	      cm->TIMING[j] = xtmp.get(1).asDouble();
	      cm->SEQUENCE[j] = j;
	    }
	  else
	    {
	      for (k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
		cm->STORED_POS[j][k] = 0.0;
	      cm->SEQUENCE[j] = -1;
	      if(j==0)
		{
		  dialog_message(GTK_MESSAGE_ERROR,
				 (char *) "Couldn't load a valid position file.", 
				 (char *) "Check the format of the supplied file.", true);
		  return;
		}
	      if(j == NUMBER_OF_STORED - 1 && xtmp.size() == NUMBER_OF_CARTESIAN_COORDINATES+1)
		{
		  dialog_message(GTK_MESSAGE_ERROR,
			      (char *) "Truncating the current sequence which is too long. You need to recompile with a greater value of NUMBER_OF_STORED", 
			      (char *) "Unfortunately maximum sequence length is not set at runtime", true);
		}
	    }
	}
  
      if(GTK_IS_TREE_VIEW (cm->treeview))
	{
	  gtk_tree_view_set_model (GTK_TREE_VIEW (cm->treeview), refresh_cartesian_list_model(cm));
	  gtk_widget_draw(GTK_WIDGET(cm->treeview), NULL);
	}
    }
  else
    dialog_message(GTK_MESSAGE_ERROR,
				(char *) "Wrong format (check estensions) of the file associated with:", 
				cm->partLabel, true);
  return;
}

/*
 * If go is clicked retrieve the current 
 * selection and move to the given position.
 * Finally update sliders
 */

void cartesianMover::go_click(GtkButton *button, cartesianMover *cm)
{
  ICartesianControl *icrt = cm->crt;	
  //get the current row index
  int i = get_cartesian_index_selection(cm);
  if (i != -1)
    {
      if (cm->TIMING[i]>0)
	{

	  double time = cm->TIMING[i];

	  Vector xd(3);  Vector eu(3);
	  xd(0) = cm->STORED_POS[i][0];
	  xd(1) = cm->STORED_POS[i][1];
	  xd(2) = cm->STORED_POS[i][2];

	  eu(0) = cm->STORED_POS[i][3]*M_PI/180.0;
	  eu(1) = cm->STORED_POS[i][4]*M_PI/180.0;
	  eu(2) = cm->STORED_POS[i][5]*M_PI/180.0;

	  Matrix R  = euler2dcm(eu);
	  Vector od = dcm2axis(R);

	  if(!icrt->goToPoseSync(xd,od,time))
	    fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
	  //else
	  //fprintf(stderr, "Sent %s, %s new orientation for %s\n", xd.toString().c_str(), od.toString().c_str(), cm->partLabel);
	  //fprintf(stderr, "It was in euler angles: %s\n", eu.toString().c_str());

	}
    }
		
  return;
}


/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */
/*
void cartesianMover::sequence_time(GtkButton *button, cartesianMover* currentPart)
{

  IPositionControl *ipos = currentPart->pos;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
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
*/
/*
 * Saves the current sequence in the 
 * correct order.
 */

//*********************************************************************************
void cartesianMover::sequence_save(GtkButton *button,  cartesianMover* cm)
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
      cm->save_to_file (filenameIn,cm);
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;
}

//*********************************************************************************

/*
 * Load a stored sequence
 */
void cartesianMover::sequence_load(GtkFileChooser *button, cartesianMover *cm )
{
  gchar* filePath = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(button));
  if (filePath == NULL) return;
  cm->load_from_file(filePath, cm);
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

void cartesianMover::sequence_cycle_time(GtkButton *button,cartesianMover* cm)
{

  ICartesianControl *icrt = cm->crt;	
  fprintf(stderr, "Retrieving sequence variables\n");
  GtkWidget *tree_view = cm->treeview;
	
  fprintf(stderr, "Preparing timing\n");
  int j, k;
  *(cm->timeout_seqeunce_rate) = (unsigned int) (cm->TIMING[0]*1000);
	
  fprintf(stderr, "Preparing inverse sequence\n");
  for (j = 0; j < NUMBER_OF_STORED; j++)
    cm->INV_SEQUENCE[j] = -1;

  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      if (cm->SEQUENCE[j]>-1 && (cm->SEQUENCE[j]<NUMBER_OF_STORED))
	{
	  cm->INV_SEQUENCE[cm->SEQUENCE[j]] = j;
	}
    }

  //if possible execute the first movement
  //SEQUENCE_ITERATOR = 0;
  fprintf(stderr, "Starting the sequence\n");
  if (cm->INV_SEQUENCE[0]!=-1 && cm->TIMING[0] >0 )
    {
      *(cm->timeout_seqeunce_id) = gtk_timeout_add(*(cm->timeout_seqeunce_rate), (GtkFunction) (cm->sequence_iterator_time), cm);
      
      double time = cm->TIMING[cm->INV_SEQUENCE[0]];

      Vector xd(3);  Vector eu(3);
      xd(0) = cm->STORED_POS[cm->INV_SEQUENCE[0]][0];
      xd(1) = cm->STORED_POS[cm->INV_SEQUENCE[0]][1];
      xd(2) = cm->STORED_POS[cm->INV_SEQUENCE[0]][2];

      eu(0) = cm->STORED_POS[cm->INV_SEQUENCE[0]][3]*M_PI/180.0;
      eu(1) = cm->STORED_POS[cm->INV_SEQUENCE[0]][4]*M_PI/180.0;
      eu(2) = cm->STORED_POS[cm->INV_SEQUENCE[0]][5]*M_PI/180.0;

      Matrix R  = euler2dcm(eu);
      Vector od = dcm2axis(R);

      if(!icrt->goToPoseSync(xd,od,time))
	fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
      //else
      //fprintf(stderr, "Sent %s, %s new orientation for %s\n", xd.toString().c_str(), od.toString().c_str(), cm->partLabel);
      
      //point the SEQUENCE ITERATOR to the next movement
      *(cm->SEQUENCE_ITERATOR) = 1;

      //deactivate all buttons
      for (k =0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
	{
	  gtk_widget_set_sensitive(cm->sliderVelocity, false);
	  gtk_widget_set_sensitive(cm->sliderArray[k], false);
	}

      if (cm->buttonGo != NULL)
	gtk_widget_set_sensitive(cm->buttonGo, false);
      if (cm->buttonSave != NULL)
	gtk_widget_set_sensitive(cm->buttonSave, false);
      if (cm->buttonLoad != NULL)
	gtk_widget_set_sensitive(cm->buttonLoad, false);
      if (cm->buttonCycTim != NULL)
	gtk_widget_set_sensitive(cm->buttonCycTim, false);
      if (cm->buttonStp != NULL)
	gtk_widget_set_sensitive(cm->buttonStp, true);
    }
  //fprintf(stderr, "Iterator was initialized \n");
  return;
}


/* Decide the next movement
 * and waits enough time so that
 * the total amount of delay corresponds
 * to the desired 
 */

bool cartesianMover::sequence_iterator_time(cartesianMover* cm)
{

  ICartesianControl *icrt = cm->crt;	
  int *SEQUENCE_TMP = cm->SEQUENCE;
  double *TIMING_TMP = cm->TIMING;
  double **STORED_POS_TMP = cm->STORED_POS;
  int *INV_SEQUENCE_TMP = cm->INV_SEQUENCE;
  GtkWidget *tree_view = cm->treeview;
  guint32* timeout_seqeunce_rate_tmp = cm->timeout_seqeunce_rate;
  guint* timeout_seqeunce_id_tmp = cm->timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR_TMP = cm->SEQUENCE_ITERATOR;

  int j = (*SEQUENCE_ITERATOR_TMP);

  if (INV_SEQUENCE_TMP[j]!=-1)
    {
      double time = cm->TIMING[cm->INV_SEQUENCE[0]];

      Vector xd(3);  Vector eu(3);
      xd(0) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][0];
      xd(1) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][1];
      xd(2) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][2];

      eu(0) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][3]*M_PI/180.0;
      eu(1) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][4]*M_PI/180.0;
      eu(2) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][5]*M_PI/180.0;

      Matrix R  = euler2dcm(eu);
      Vector od = dcm2axis(R);

      if(!icrt->goToPoseSync(xd,od,time))
	fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
      //else
      //fprintf(stderr, "Sent %s, %s new orientation for %s\n", xd.toString().c_str(), od.toString().c_str(), cm->partLabel);


      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) (cm->sequence_iterator_time), cm);
    }
  else
    {
      //restart the sequence if finished
      *SEQUENCE_ITERATOR_TMP = 0;
      j = 0;

      double time = cm->TIMING[cm->INV_SEQUENCE[0]];

      Vector xd(3);  Vector eu(3);
      xd(0) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][0];
      xd(1) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][1];
      xd(2) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][2];

      eu(0) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][3]*M_PI/180.0;
      eu(1) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][4]*M_PI/180.0;
      eu(2) = cm->STORED_POS[INV_SEQUENCE_TMP[j]][5]*M_PI/180.0;

      Matrix R  = euler2dcm(eu);
      Vector od = dcm2axis(R);

      if(!icrt->goToPoseSync(xd,od,time))
	fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
      //else
      //fprintf(stderr, "Sent %s, %s new orientation for %s\n", xd.toString().c_str(), od.toString().c_str(), cm->partLabel);

      (*SEQUENCE_ITERATOR_TMP)++;
      *timeout_seqeunce_rate_tmp = (unsigned int) (TIMING_TMP[j]*1000);
      gtk_timeout_remove(*timeout_seqeunce_id_tmp);
      *timeout_seqeunce_id_tmp = gtk_timeout_add(*timeout_seqeunce_rate_tmp, (GtkFunction) (cm->sequence_iterator_time), cm);
    }

  return false;
}


/*
 * If sequence button is pressed the 
 * sequence of movements is executed
 */

void cartesianMover::sequence_stop(GtkButton *button,cartesianMover* cm)
{
  //reactivate all buttons
  int k;
  for (k =0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
    {
      gtk_widget_set_sensitive(cm->sliderVelocity, true);
      gtk_widget_set_sensitive(cm->sliderArray[k], true);
    }

  //fprintf(stderr, "Enabling buttons...");
  if (cm->buttonGo != NULL)
    gtk_widget_set_sensitive(cm->buttonGo, true);
  if (cm->buttonSave != NULL)
    gtk_widget_set_sensitive(cm->buttonSave, true);
  if (cm->buttonLoad != NULL)
    gtk_widget_set_sensitive(cm->buttonLoad, true);
  if (cm->buttonCycTim != NULL)
    gtk_widget_set_sensitive(cm->buttonCycTim, true);
  if (cm->buttonStp != NULL)
    gtk_widget_set_sensitive(cm->buttonStp, true);
    //fprintf(stderr, "disabled...");     

  gtk_timeout_remove(*(cm->timeout_seqeunce_id));
  return;
}
