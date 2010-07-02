#include "include/allPartsWindow.h"

#include <stdlib.h>
#include <string.h>

//*********************************************************************************
// This callback starts all sequences (time based)
void run_all_parts (GtkButton *button, partMover** currentPartMover)
{
  int i; 
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->run_all(NULL, currentPartMover[i]);
  return;
}

//*********************************************************************************
// This callback starts all sequences (time based)
void home_all_parts (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->home_all(NULL, currentPartMover[i]);
  return;
}

//*********************************************************************************
// This callback moves all selected
void go_all_click (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->go_click(NULL, currentPartMover[i]);
  return;
}


//*********************************************************************************
// This callback starts all sequences (time based)
void sequence_all_click_time (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->sequence_time(NULL, currentPartMover[i]);
  return;
}

//*********************************************************************************
// This callback starts all sequences
void sequence_all_click (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->sequence_click(NULL, currentPartMover[i]);
  return;
}

//*********************************************************************************
// This callback saves all sequences
void sequence_all_save (GtkButton *button, partMover** currentPartMover)
{
        
  FILE* fileAll;
  char buffer[800];

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
      int i;
    	  
      filenameIn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
	currentPartMover[i]->save_to_file(filenameIn,currentPartMover[i]);

      //store the file describing the poses
      fileAll = fopen(filenameIn , "w");
      for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
	{
	  sprintf(buffer, "[%s] \n", currentPartMover[i]->partLabel);
	  fprintf(fileAll, "%s", buffer);
	  //Specific part filename
	  sprintf(buffer, "%s.pos%s \n\n", filenameIn, currentPartMover[i]->partLabel);
	  fprintf(fileAll, "%s", buffer);
	}
      fclose(fileAll);
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;

}

//*********************************************************************************
// This callback saves all sequences
void sequence_crt_all_save (GtkButton *button, cartesianMover** cm)
{
        
  FILE* fileAll;
  char buffer[800];

  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Save Cartesian File",
					(GtkWindow*) window,
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
	
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filenameIn; 
      int i;
    	  
      filenameIn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      for (i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
	cm[i]->save_to_file(filenameIn,cm[i]);

      //store the file describing the poses
      fileAll = fopen(filenameIn , "w");
      for (i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
	{
	  sprintf(buffer, "[%s] \n", cm[i]->partLabel);
	  fprintf(fileAll, "%s", buffer);
	  //Specific part filename
	  sprintf(buffer, "%s.crt%s\n\n", filenameIn, cm[i]->partLabel);
	  fprintf(fileAll, "%s", buffer);
	}
      fclose(fileAll);
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;

}

//*********************************************************************************
// This callback loads all sequences
void sequence_all_load (GtkButton *button, partMover** currentPartMover)
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Open File",
					(GtkWindow*) window,
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
	
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filenameIn; 
      int i, lengthStr;
    	  
      filenameIn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	  
      //store the file describing the poses
      for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
	{
	  Property p;
	  bool fileExists;
	  char buffer[800];

	  fileExists = p.fromConfigFile(filenameIn);
	  if (fileExists)
	    {
	      Bottle& xtmp = p.findGroup(currentPartMover[i]->partLabel);
	      
	      sprintf(buffer, "%s",(const char*)xtmp.get(1).toString());
	      lengthStr = strlen(buffer);
	      *(buffer + (lengthStr-1)*sizeof(char)) = (char) NULL;
	      currentPartMover[i]->load_from_file(buffer+(sizeof(char)),currentPartMover[i]); 
	    }
	  else
	    {
	      dialog_message(GTK_MESSAGE_ERROR,
			     (char *)"Couldn't find the file describing the positions ", 
			     (char *)"associated to the currently activated parts", true);
	    }
	}
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;

  
  //int i;
  //for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
  //	currentPartMover[i]->sequence_load(NULL, currentPartMover[i]);
  //return;
}

//*********************************************************************************
// This callback loads all cartesian sequences
void sequence_crt_all_load (GtkButton *button, cartesianMover** cm)
{
  GtkWidget *dialog;
  dialog = gtk_file_chooser_dialog_new ("Open File",
					(GtkWindow*) window,
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
	
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char *filenameIn; 
      int i, lengthStr;
    	  
      filenameIn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	  
      //store the file describing the poses
      for (i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
	{
	  Property p;
	  bool fileExists;
	  char buffer[800];

	  fileExists = p.fromConfigFile(filenameIn);
	  if (fileExists)
	    {
	      Bottle& xtmp = p.findGroup(cm[i]->partLabel);
	      sprintf(buffer, "%s",xtmp.get(1).toString().c_str());
	      lengthStr = strlen(buffer);
	      *(buffer + (lengthStr-1)*sizeof(char)) = (char) NULL;
	      fprintf(stderr, "Loading from %s\n",buffer+sizeof(char));
	      cm[i]->load_from_file(buffer+sizeof(char),cm[i]);
	    }
	  else
	    {
	      dialog_message(GTK_MESSAGE_ERROR,
			     (char *)"Couldn't find the file describing the positions ", 
			     (char *)"associated to the currently activated parts", true);
	    }
	}
      g_free (filenameIn);
    }
  gtk_widget_destroy (dialog);
	
  return;
}

//*********************************************************************************
// This callback cycles all sequences
void sequence_all_cycle (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->sequence_cycle(NULL, currentPartMover[i]);

  gtk_widget_set_sensitive(buttonGoAll, false);
  gtk_widget_set_sensitive(buttonSeqAll, false);
  gtk_widget_set_sensitive(buttonSeqAllTime, false);
  gtk_widget_set_sensitive(buttonSeqAllSave, false);
  gtk_widget_set_sensitive(buttonSeqAllLoad, false);
  gtk_widget_set_sensitive(buttonSeqAllCycle, false);
  gtk_widget_set_sensitive(buttonSeqAllCycleTime, false);
  gtk_widget_set_sensitive(buttonRunAllParts, false);
  gtk_widget_set_sensitive(buttonHomeAllParts, false);
  //gtk_widget_set_sensitive(buttonSeqAllStopTime, false);

  return;
}

//*********************************************************************************
// This callback cycles all sequences in time based
void sequence_all_cycle_time (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->sequence_cycle_time(NULL, currentPartMover[i]);

  //fprintf(stderr, "Setting go all insensitive \n");
  gtk_widget_set_sensitive(buttonGoAll, false);
  //fprintf(stderr, "Setting seq all insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAll, false);
  //fprintf(stderr, "Setting seq all time insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAllTime, false);
  //fprintf(stderr, "Setting seq all save insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAllSave, false);
  //fprintf(stderr, "Setting seq all load insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAllLoad, false);
  //fprintf(stderr, "Setting seq all cycle insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAllCycle, false);
  //fprintf(stderr, "Setting seq all cycle time insensitive \n");
  gtk_widget_set_sensitive(buttonSeqAllCycleTime, false);
  //fprintf(stderr, "Setting run all insensitive \n");
  gtk_widget_set_sensitive(buttonRunAllParts, false);
  //fprintf(stderr, "Setting home all insensitive \n");
  gtk_widget_set_sensitive(buttonHomeAllParts, false);

  return;
}

//*********************************************************************************
// This callback cycles all sequences in time based
void sequence_crt_all_cycle_time (GtkButton *button, cartesianMover** cm)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
    cm[i]->sequence_cycle_time(NULL, cm[i]);


  gtk_widget_set_sensitive(buttonCrtSeqAllSave, false);
  gtk_widget_set_sensitive(buttonCrtSeqAllLoad, false);
  gtk_widget_set_sensitive(buttonCrtSeqAllCycleTime, false);

  return;
}

//*********************************************************************************
// This callback stops all cycling sequences
void sequence_all_stop (GtkButton *button, partMover** currentPartMover)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
    currentPartMover[i]->sequence_stop(NULL, currentPartMover[i]);

  gtk_widget_set_sensitive(buttonGoAll, true);
  gtk_widget_set_sensitive(buttonSeqAll, true);
  gtk_widget_set_sensitive(buttonSeqAllTime, true);
  gtk_widget_set_sensitive(buttonSeqAllSave, true);
  gtk_widget_set_sensitive(buttonSeqAllLoad, true);
  gtk_widget_set_sensitive(buttonSeqAllCycle, true);
  gtk_widget_set_sensitive(buttonSeqAllCycleTime, true);
  //gtk_widget_set_sensitive(buttonSeqAllStopTime, true);
  gtk_widget_set_sensitive(buttonRunAllParts, true);
  gtk_widget_set_sensitive(buttonHomeAllParts, true);
  return;
}

//*********************************************************************************
// This callback stops all cycling sequences
void sequence_crt_all_stop (GtkButton *button, cartesianMover** cm)
{
  int i;
  for (i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
    cm[i]->sequence_stop(NULL, cm[i]);

  return;
}

