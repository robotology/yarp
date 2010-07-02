#include "include/robotMotorGui.h"
#include "include/cartesianMover.h"

#include <stdlib.h>

//static void destroy_main (GtkWindow *window,	gpointer   user_data)
static void destroy_main (GtkWindow *window,	cartesianMover* cm)
{
  //fprintf(stderr, "Reinitializing all pointers\n");
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
 * Display the list containing 
 * the positions sotred so far
 */

GtkTreeModel* refresh_cartesian_list_model(cartesianMover *cm)
{
  const int ADDITIONAL_COLUMNS = 2;
	
  GType types[NUMBER_OF_CARTESIAN_COORDINATES + ADDITIONAL_COLUMNS];
  GtkListStore *store;
  GtkTreeIter iter;
	
  GValue valore={0};
  g_value_init(&valore, G_TYPE_STRING);
	
  int cols[NUMBER_OF_CARTESIAN_COORDINATES + ADDITIONAL_COLUMNS];
  int k,j;
	
  for (k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES + ADDITIONAL_COLUMNS; k++)
    {
      types[k] = G_TYPE_STRING;
      cols[k]  = k;
    }
	
  store = gtk_list_store_newv ( NUMBER_OF_CARTESIAN_COORDINATES + ADDITIONAL_COLUMNS, types);
  //gtk_list_store_set   (store, &iter, 0, 0.0, 1, 0.1, 2, 0.2, 3, 0.3, 4, 0.4, 5, 0.5, 6, 0.6, 7, 0.7, 8, 0.8 , 9, 0.9, 10, 1.0, 11, 1.1, 12, 1.2, 13, 1.3, 14, 1.4, 15, 1.5, -1);
  char buffer[800];
	
  
  for (j = 0; j < NUMBER_OF_STORED; j++)
    {
      gtk_list_store_append (store, &iter);
		
      for (k = 0; k < ADDITIONAL_COLUMNS; k++)
	{
	  if (k == POS_SEQUENCE)
	      sprintf(buffer, "%d", cm->SEQUENCE[j]);
	  else
	    sprintf(buffer, "%.1f", cm->TIMING[j]);
	  g_value_set_string(&valore, buffer);
	  gtk_list_store_set_value(store, &iter, cols[k], &valore);
	}
		
      for (k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
	{
	  sprintf(buffer, "%.1f", cm->STORED_POS[j][k]);
	  g_value_set_string(&valore, buffer);
	  gtk_list_store_set_value(store, &iter, cols[k+ADDITIONAL_COLUMNS], &valore);
	}
    }
  
  return GTK_TREE_MODEL (store);
}


/*
 * Retrieve index of the current selection
 */

int get_cartesian_index_selection(cartesianMover *cm)
{
  GtkTreeIter iter;
  GtkTreeSelection *treeSelection 	= NULL;
  GtkWidget *tree_view = cm->treeview;
  treeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
  GtkTreeModel* myModel = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
	
  bool selected = gtk_tree_selection_get_selected (treeSelection, NULL, &iter);
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
 * If line is double-clicked insert the current
 * position in the correponding list position
 */


void cartesianMover::cartesian_line_click(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, cartesianMover *cm)
{

  ICartesianControl *icrt = cm->crt;
  int *i = gtk_tree_path_get_indices (path);
  double currPos[NUMBER_OF_CARTESIAN_COORDINATES];

  Vector x;
  Vector axis;
  if(!icrt->getPose(x,axis))
    fprintf(stderr, "Troubles in getting the cartesian pose for %s", cm->partLabel);
  

  Matrix R = axis2dcm(axis);
  Vector eu = dcm2euler(R);
  //fprintf(stderr, "Storing euler angles: %s...", eu.toString().c_str());
  //fprintf(stderr, "...corrsponding to axis: %s", axis.toString().c_str());

  for (int k =0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
    {
      if(k<3)
	cm->STORED_POS[*i][k] = x(k);
      else
	cm->STORED_POS[*i][k] = eu(k-3)*180/M_PI;
    }
	
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_cartesian_list_model(cm));
  gtk_widget_draw(GTK_WIDGET(tree_view), NULL);
	
  return;
}


/* 
 * Initilize the variables used by the table
 */

void cartesianMover::init_cartesian_table()
{
  SEQUENCE = new int[NUMBER_OF_STORED];
  INV_SEQUENCE = new int[NUMBER_OF_STORED];
  TIMING = new double[NUMBER_OF_STORED];
  STORED_POS = new double*[NUMBER_OF_STORED];
  SEQUENCE_ITERATOR = new int;
  timeout_seqeunce_rate = new guint32;
  timeout_seqeunce_id = new guint;

  for (int i = 0; i < NUMBER_OF_STORED; i++)
    {
      SEQUENCE[i]=-1;
      TIMING[i]=-0.1;
      STORED_POS[i] = new double[NUMBER_OF_CARTESIAN_COORDINATES];
      for (int j = 0; j < NUMBER_OF_CARTESIAN_COORDINATES; j++)
	STORED_POS[i][j] = 0.0;
    }
  //fprintf(stderr, "Table was initialized\n");
}




/*
 * If sequence order has been edited stores the new time
 * and redisplay the list
 */

void cartesianMover::edited_cartesian_sequence (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, cartesianMover *cm)
{

  GtkWidget *tree_view = cm->treeview;

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
  int i = get_cartesian_index_selection(cm);
  if (i != -1)
    cm->SEQUENCE[i] = atoi(new_text);
	
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_cartesian_list_model(cm));
  gtk_widget_draw(tree_view, NULL);
}



/*
 * If time has been edited stores the new time
 * and redisplay the list
 */


void cartesianMover::edited_cartesian_timing (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, cartesianMover *cm)
{
  GtkWidget *tree_view = cm->treeview;


  //retrieve new val from edited
  gdouble  new_val = atof (new_text);
	
  //get the current row index
  int i = get_cartesian_index_selection(cm);
  //fprintf(stderr, "Getting index %d", i);
  if (i != -1)
    {
      if (new_val > 0)
	cm->TIMING[i] = new_val;
      else
	dialog_message(GTK_MESSAGE_ERROR, (char *) "Timing must be positive", (char *) "Change your selection", true);
    }
	
  //redisplay list
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), refresh_cartesian_list_model(cm));
  gtk_widget_draw(tree_view, NULL);
}



void add_cartesian_columns (GtkTreeView *tree_view, cartesianMover* cm)
{
  int k;
  int ADDITIONAL_COLUMNS = 2;
  char buffer[40];
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  //GtkTreeModel *myModel = gtk_tree_view_get_model (tree_view);

  //Sequence order
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=true;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=true;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;

  g_signal_connect (renderer, "edited", G_CALLBACK (cm->edited_cartesian_sequence), cm);
	
  column = gtk_tree_view_column_new_with_attributes ("Sequence",
						     renderer,
						     "text",
						     0,
						     NULL);
	
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 80);
  gtk_tree_view_append_column (tree_view, column);
	
	
  //Sequence timing
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=true;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=true;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;

  g_signal_connect (renderer, "edited", G_CALLBACK (cm->edited_cartesian_timing), cm);
	
  column = gtk_tree_view_column_new_with_attributes ("Timing",
						     renderer,
						     "text",
						     1,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 80);
  gtk_tree_view_append_column (tree_view, column);
	
  //Sequence description
  renderer = gtk_cell_renderer_text_new ();
  GTK_CELL_RENDERER_TEXT(renderer)->editable=false;
  GTK_CELL_RENDERER_TEXT(renderer)->editable_set=false;
  renderer->mode=GTK_CELL_RENDERER_MODE_EDITABLE;
	
  //fprintf(stderr, "Current add_columns received %d\n", NUMBER_OF_JOINTS);
	
  for (k =0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
    {
      if(k<3)
	sprintf(buffer, "x%d", k);
      else
	sprintf(buffer, "th%d", k-3);

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
 * Open table
 */

void cartesianMover::cartesian_table_open(GtkButton *button, cartesianMover *cm)
{
  
  //int *joint = currentClassData->indexPointer;
  /*ipos = currentPart->pos;
  iiencs = currentPart->iencs;
  iamp   = currentPart->amp;
  ipid   = currentPart->pid;
  */
  GtkWidget *winTable = NULL;
  
  //adding a popup window
  winTable = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (winTable), cm->partLabel);
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
  GtkTreeModel *myModel = refresh_cartesian_list_model(cm);

  //create tree view
  GtkTreeSelection *treeSelection2 	= NULL;
  GtkWidget *myTreeview = gtk_tree_view_new_with_model (myModel);
  cm->treeview=myTreeview;

  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (myTreeview), TRUE);
  treeSelection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(myTreeview));
  gtk_tree_selection_set_mode(treeSelection2, GTK_SELECTION_SINGLE);
  g_signal_connect (myTreeview, "row-activated", G_CALLBACK (cm->cartesian_line_click), cm);
				
  //g_object_unref (myModel);
  gtk_container_add (GTK_CONTAINER (sw), myTreeview);
		
  // add columns to the tree view
  add_cartesian_columns (GTK_TREE_VIEW (myTreeview), cm);

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
  cm->buttonGo = gtk_button_new_with_mnemonic ("Go!");
  gtk_widget_set_size_request 	(cm->buttonGo, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), cm->buttonGo, 30, 30);
  g_signal_connect (cm->buttonGo, "clicked", G_CALLBACK (go_click), cm);

  //Button "Save Sequence" in the panel
  cm->buttonSave = gtk_button_new_with_mnemonic ("Save");
  gtk_widget_set_size_request 	(cm->buttonSave, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), cm->buttonSave, 30, 130);
  g_signal_connect (cm->buttonSave, "clicked", G_CALLBACK (sequence_save), cm);

  //Button "Load Sequence" in the panel
  cm->buttonLoad = gtk_file_chooser_button_new ("Load Seqeunce", GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_set_size_request 	(cm->buttonLoad, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), cm->buttonLoad, 30, 180);
  g_signal_connect (cm->buttonLoad, "selection-changed", G_CALLBACK (sequence_load), cm);
  //g_signal_connect (button4, "clicked", G_CALLBACK (sequence_load),this);

  //Button "Cycle Sequence (time)" in the panel
  cm->buttonCycTim = gtk_button_new_with_mnemonic ("Cycle (time)");
  gtk_widget_set_size_request 	(cm->buttonCycTim, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), cm->buttonCycTim, 30, 280);
  g_signal_connect (cm->buttonCycTim, "clicked", G_CALLBACK (cm->sequence_cycle_time), cm);


  //Button "Stop Sequence" in the panel
  cm->buttonStp = gtk_button_new_with_mnemonic ("Stop");
  gtk_widget_set_size_request 	(cm->buttonStp, 100, 30);
  gtk_fixed_put (GTK_FIXED (inv1), cm->buttonStp, 30, 330);
  g_signal_connect (cm->buttonStp, "clicked", G_CALLBACK (cm->sequence_stop),cm);


  //connection to the destroyer
  //g_signal_connect (winTable, "destroy",G_CALLBACK (destroy_main), &winTable);
  g_signal_connect (winTable, "destroy",G_CALLBACK (destroy_main), cm);
  //make it visible
  if (!GTK_WIDGET_VISIBLE (winTable))
    gtk_widget_show_all (winTable);
  else
    {
      //fprintf(stderr, "Reinitializing all pointers\n");
      //currentPart->button8 = NULL;
      gtk_widget_destroy (winTable);
      winTable = NULL;
    }
	
  gtk_main ();

  return;
}
