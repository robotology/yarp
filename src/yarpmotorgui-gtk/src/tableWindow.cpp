#include "include/robotMotorGui.h"
#include "include/partMover.h"

#include <stdlib.h>

//static void destroy_main (GtkWindow *window,	gpointer   user_data)
static void destroy_main (GtkWindow *window,	partMover* currentPart)
{
  //fprintf(stderr, "Reinitializing all pointers\n");
  currentPart->button1 = NULL;
  currentPart->button2 = NULL;
  currentPart->button3 = NULL;
  currentPart->button4 = NULL;
  currentPart->button5 = NULL;
  currentPart->button6 = NULL;
  currentPart->button7 = NULL;
  currentPart->button8 = NULL;
  gtk_widget_destroy (GTK_WIDGET(window));
  gtk_main_quit ();
}



//*********************************************************************************
// This callback exits from the table dialog
static void destroy_win (GtkButton *button, GtkWindow *window)
{
  gtk_widget_destroy (GTK_WIDGET(window));
  gtk_main_quit ();
}

/*
 * If line is double-clicked insert the current
 * position in the correponding list position
 */

void partMover::line_click(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gtkClassData* currentClassData)
{

  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;

  int *i = gtk_tree_path_get_indices (path);
  double currPos[MAX_NUMBER_OF_JOINTS];
  double **STORED_POS_TMP;
  double **STORED_VEL_TMP;
	

  STORED_POS_TMP = currentPart->STORED_POS;
  STORED_VEL_TMP = currentPart->STORED_VEL;
  while (!iiencs->getEncoders(currPos))
    Time::delay(0.001);
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
	
  for (int k =0; k < NUMBER_OF_JOINTS; k++)
    {
      STORED_VEL_TMP[*i][k] = gtk_range_get_value((GtkRange*) sliderVelAry[k]);
      STORED_POS_TMP[*i][k] = currPos[k];
    }
	
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_position_list_model(currentPart));
  gtk_widget_draw(GTK_WIDGET(tree_view), NULL);
	
  return;
}

/* 
 * Display the list containing 
 * the positions sotred so far
 */
GtkTreeModel * partMover::refresh_position_list_model(partMover* currentPart)
{
  GtkWidget **sliderVelAry = currentPart->sliderVelArray;
  IPositionControl *ipos = currentPart->pos;
  IEncoders *iiencs = currentPart->iencs;
  IAmplifierControl *iamp = currentPart->amp;
  IPidControl *ipid = currentPart->pid;
  int *SEQUENCE_TMP = currentPart->SEQUENCE;
  double *TIMING_TMP = currentPart->TIMING;
  double **STORED_POS_TMP = currentPart->STORED_POS;
	
  GType types[MAX_NUMBER_OF_JOINTS + ADDITIONAL_COLUMNS];
  GtkListStore *store;
  GtkTreeIter iter;
	
  GValue valore={0};
  g_value_init(&valore, G_TYPE_STRING);
	
  int cols[MAX_NUMBER_OF_JOINTS + ADDITIONAL_COLUMNS];
  int k,j;
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
	
  for (k = 0; k < NUMBER_OF_JOINTS + ADDITIONAL_COLUMNS; k++)
    {
      types[k] = G_TYPE_STRING;
      cols[k]  = k;
    }
	
  store = gtk_list_store_newv ( NUMBER_OF_JOINTS + ADDITIONAL_COLUMNS, types);
	
	
  //gtk_list_store_set   (store, &iter, 0, 0.0, 1, 0.1, 2, 0.2, 3, 0.3, 4, 0.4, 5, 0.5, 6, 0.6, 7, 0.7, 8, 0.8 , 9, 0.9, 10, 1.0, 11, 1.1, 12, 1.2, 13, 1.3, 14, 1.4, 15, 1.5, -1);
  char buffer[800];
	
	
  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      gtk_list_store_append (store, &iter);
		
      for (k = 0; k < ADDITIONAL_COLUMNS; k++)
	{
	  if (k == POS_SEQUENCE)
	    sprintf(buffer, "%d", SEQUENCE_TMP[j]);
	  else
	    sprintf(buffer, "%.1f", TIMING_TMP[j]);
	  g_value_set_string(&valore, buffer);
	  gtk_list_store_set_value(store, &iter, cols[k], &valore);
	}
		
      for (k = 0; k < NUMBER_OF_JOINTS; k++)
	{
	  sprintf(buffer, "%.1f", STORED_POS_TMP[j][k]);
	  g_value_set_string(&valore, buffer);
	  gtk_list_store_set_value(store, &iter, cols[k+ADDITIONAL_COLUMNS], &valore);
	}
    }
  return GTK_TREE_MODEL (store);
}



/*
 * If sequence order has been edited stores the new time
 * and redisplay the list
 */

void partMover::edited_sequence (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, gtkClassData* currentClassData)
{

  //fprintf(stderr, "Sequence was edited \n");
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
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

  gint  new_val = atoi (new_text);
  if	 (new_val <-1 || new_val>= NUMBER_OF_STORED) return;
  if	 (new_val == NUMBER_OF_STORED -1)
    {
	 dialog_message(GTK_MESSAGE_ERROR,
			      (char *) "Please do not use the entire table (leave at least one row). Otherwise increase NUMBER_OF_STORED", 
			      (char *) "Unfortunately maximum sequence length is not set at runtime (recompile)", true);
	 return;
    }
  //---
	
  //get the current row index
  int i = get_index_selection(currentPart);
  if (i != -1)
    SEQUENCE_TMP[i] = atoi(new_text);
	
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_position_list_model(currentPart));
  gtk_widget_draw(tree_view, NULL);
}

/*
 * If time has been edited stores the new time
 * and redisplay the list
 */
void partMover::edited_timing (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  int * joint = currentClassData->indexPointer;
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


  //retrieve new val from edited
  gdouble  new_val = atof (new_text);
	
  //get the current row index
  int i = get_index_selection(currentPart);
  if (i != -1)
    {
      if (new_val > 0)
	TIMING_TMP[i] = new_val;
      else
	dialog_message(GTK_MESSAGE_ERROR, (char *) "Timing must be positive", (char *) "Change your selection", true);
    }
	
  //redisplay list
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_position_list_model(currentPart));
  gtk_widget_draw(tree_view, NULL);
}


void partMover::add_columns (GtkTreeView *tree_view, gtkClassData* currentClassData)
{
  int k;
  char buffer[40];
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  //GtkTreeModel *myModel = gtk_tree_view_get_model (tree_view);
  partMover *currentPart = currentClassData->partPointer;
  IPositionControl *ipos = currentPart->pos;

  //Sequence order
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=true;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=true;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;

  g_signal_connect (renderer, "edited", G_CALLBACK (edited_sequence), currentClassData);
	
  column = gtk_tree_view_column_new_with_attributes ("Sequence",
						     renderer,
						     "text",
						     POS_SEQUENCE,
						     NULL);
	
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 80);
  gtk_tree_view_append_column (tree_view, column);
	
	
  //Sequence timing
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=true;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=true;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;

  g_signal_connect (renderer, "edited", G_CALLBACK (edited_timing), currentClassData);
	
  column = gtk_tree_view_column_new_with_attributes ("Timing",
						     renderer,
						     "text",
						     TIM_SEQUENCE,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 80);
  gtk_tree_view_append_column (tree_view, column);
	
  //Sequence description
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=false;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=false;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;
	
  int NUMBER_OF_JOINTS;
  ipos->getAxes(&NUMBER_OF_JOINTS);
  //fprintf(stderr, "Current add_columns received %d\n", NUMBER_OF_JOINTS);
	
  for (k =0; k < NUMBER_OF_JOINTS; k++)
    {
      sprintf(buffer, "J%d", k);
		
      column = gtk_tree_view_column_new_with_attributes (buffer,
							 renderer,
							 "text",
							 k+ADDITIONAL_COLUMNS,
							 NULL);
      gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),GTK_TREE_VIEW_COLUMN_FIXED);
      gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 40);
      //gtk_tree_view_column_set_sort_column_id (column, COLUMN_ADD_INFO);
      gtk_tree_view_append_column (tree_view, column);
    }
}


//*********************************************************************************
// This callback exits from the gtk_main() main loop when the main window is closed
//void destroy_main (GtkWindow *window,	gpointer   user_data)
//{
//  gtk_widget_destroy (GTK_WIDGET(window));
//  gtk_main_quit ();
//}

/*
 * Retrieve index of the current selection
 */

int partMover::get_index_selection(partMover *currentPart)
{
  GtkTreeIter iter					;
  GtkTreeSelection *treeSelection 	= NULL;
  GtkWidget *tree_view = currentPart->treeview;
  treeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
  GtkTreeModel* myModel = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
	
  bool selected = (gtk_tree_selection_get_selected (treeSelection, NULL, &iter)?true:false);
  //gchar *buffer[10000];
  //gtk_tree_model_get(myModel, &iter, 0, buffer, -1);
	
	
  if(selected==false)
    {
      dialog_message(GTK_MESSAGE_ERROR,(char *) "Select a entry in the table", (char *) "before performing a movement", true);
      return -1;
    }
  else
    {
      GtkTreePath* treePath = gtk_tree_model_get_path(myModel, &iter);
      int *i = gtk_tree_path_get_indices (treePath);
      //gtk_tree_path_free (treePath);
      return (*i);
    }
}


/*
 * Open table
 */

void partMover::table_open(GtkButton *button, gtkClassData* currentClassData)
{
  partMover *currentPart = currentClassData->partPointer;
  //int *joint = currentClassData->indexPointer;
  /*ipos = currentPart->pos;
  iiencs = currentPart->iencs;
  iamp   = currentPart->amp;
  ipid   = currentPart->pid;
  */
  GtkWidget *winTable = NULL;
  
  //adding a popup window
  winTable = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (winTable), currentPart->partLabel);
  gtk_window_set_default_size((GtkWindow*) winTable, 300, 300);
  
  //creating a new box
  GtkWidget *bottom_hbox = gtk_hbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (bottom_hbox), 10);
  gtk_container_add (GTK_CONTAINER (winTable), bottom_hbox);
		
  //In the bottom frame there is:
  //1) the list of the cards
  GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (bottom_hbox), sw, TRUE, TRUE, 0);

  //GtkTreeModel *myModel = currentPart->model;
  //GtkWidget *myTreeview = currentPart->treeview;
  GtkTreeModel *myModel = refresh_position_list_model(currentPart);
  currentPart->model=myModel;

  //create tree view
  GtkTreeSelection *treeSelection2 	= NULL;
  GtkWidget *myTreeview = gtk_tree_view_new_with_model (myModel);
  currentPart->treeview=myTreeview;

  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (myTreeview), TRUE);
  treeSelection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(myTreeview));
  gtk_tree_selection_set_mode(treeSelection2, GTK_SELECTION_SINGLE);
  g_signal_connect (myTreeview, "row-activated", G_CALLBACK (line_click), currentClassData);
				
  g_object_unref (myModel);
  gtk_container_add (GTK_CONTAINER (sw), myTreeview);
		
  // add columns to the tree view
  add_columns (GTK_TREE_VIEW (myTreeview), currentClassData);

  //creating a new box
  GtkWidget *inv1 = gtk_fixed_new();
  GtkWidget *frame1 = gtk_frame_new ("Commands");
  gtk_container_add (GTK_CONTAINER (bottom_hbox), inv1);
  gtk_widget_set_size_request 	(frame1, 150, 400);
  gtk_fixed_put (GTK_FIXED (inv1), frame1, 10, 10);
  //GtkWidget *bottom_box = gtk_vbox_new (FALSE, 8);
  //gtk_container_set_border_width (GTK_CONTAINER (bottom_box), 10);
  //gtk_container_add (GTK_CONTAINER (bottom_hbox), bottom_box);

  //Button Go! in the panel
  GtkWidget *buttonGo = gtk_button_new_with_mnemonic ("Go!");
  gtk_widget_set_size_request 	(buttonGo, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonGo, 30, 30);
  g_signal_connect (buttonGo, "clicked", G_CALLBACK (go_click), currentPart);
  currentPart->button1 = buttonGo;

  //Button "Run Sequence" in the panel
  GtkWidget *buttonRun = gtk_button_new_with_mnemonic ("Run (time)");
  gtk_widget_set_size_request 	(buttonRun, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonRun, 30, 80);
  g_signal_connect (buttonRun, "clicked", G_CALLBACK (sequence_time),currentPart);
  currentPart->button7 = buttonRun;

  //Button "Save Sequence" in the panel
  GtkWidget *buttonSave = gtk_button_new_with_mnemonic ("Save");
  gtk_widget_set_size_request 	(buttonSave, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonSave, 30, 130);
  g_signal_connect (buttonSave, "clicked", G_CALLBACK (sequence_save),currentPart);
  currentPart->button3 = buttonSave;		

  //Button "Load Sequence" in the panel
  GtkWidget *buttonLoad = gtk_file_chooser_button_new ("Load Seqeunce", GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_set_size_request 	(buttonLoad, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonLoad, 30, 180);
  g_signal_connect (buttonLoad, "selection-changed", G_CALLBACK (sequence_load), currentPart);
  //g_signal_connect (button4, "clicked", G_CALLBACK (sequence_load),this);
  currentPart->button4 = buttonLoad;

  //Button "Run Sequence" in the panel
  GtkWidget *buttonSeq = gtk_button_new_with_mnemonic ("Run Seq");
  gtk_widget_set_size_request 	(buttonSeq, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonSeq, 30, 230);
  g_signal_connect (buttonSeq, "clicked", G_CALLBACK (sequence_click),currentPart);
  currentPart->button2 = buttonSeq;

  //Button "Cycle Sequence" in the panel
  GtkWidget *buttonCyc = gtk_button_new_with_mnemonic ("Cycle");
  gtk_widget_set_size_request 	(buttonCyc, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonCyc, 30, 230);
  g_signal_connect (buttonCyc, "clicked", G_CALLBACK (sequence_cycle),currentPart);
  currentPart->button5 = buttonCyc;

  //Button "Cycle Sequence (time)" in the panel
  GtkWidget *buttonCycTim = gtk_button_new_with_mnemonic ("Cycle (time)");
  gtk_widget_set_size_request 	(buttonCycTim, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonCycTim, 30, 280);
  g_signal_connect (buttonCycTim, "clicked", G_CALLBACK (sequence_cycle_time),currentPart);
  currentPart->button8 = buttonCycTim;

  //Button "Stop Sequence" in the panel
  GtkWidget *buttonStp = gtk_button_new_with_mnemonic ("Stop");
  gtk_widget_set_size_request 	(buttonStp, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), buttonStp, 30, 330);
  g_signal_connect (buttonStp, "clicked", G_CALLBACK (sequence_stop),currentPart);
  currentPart->button6 = buttonStp;

  //connection to the destroyer
  //g_signal_connect (winTable, "destroy",G_CALLBACK (destroy_main), &winTable);
  g_signal_connect (winTable, "destroy",G_CALLBACK (destroy_main), currentPart);
  //make it visible
  if (!GTK_WIDGET_VISIBLE (winTable))
    gtk_widget_show_all (winTable);
  else
    {
      //fprintf(stderr, "Reinitializing all pointers\n");
      currentPart->button1 = NULL;
      currentPart->button2 = NULL;
      currentPart->button3 = NULL;
      currentPart->button4 = NULL;
      currentPart->button5 = NULL;
      currentPart->button6 = NULL;
      currentPart->button7 = NULL;
      currentPart->button8 = NULL;
      gtk_widget_destroy (winTable);
      winTable = NULL;
    }
	
  gtk_main ();

  return;
}
