#ifndef CARTESIANMOVER_H
#define CARTESIANMOVER_H

#include <math.h>
///////////YARP//////////
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/ResourceFinder.h>
///////////ICUB///////////
#include <iCub/ctrl/ctrlMath.h>
///////////GTK///////////
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "gtkMessages.h"
//strings
#include <string>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace ctrl;
using namespace yarp::math;

#define NUMBER_OF_CARTESIAN_COORDINATES 6
#define UPDATE_TIME 500

struct gtkClassData;

class cartesianMover{
 private:
  GtkWidget *vbox;
  PolyDriver *partDd;

  //cartesian interface
  ICartesianControl *crt;

  int *index;

  GtkWidget **frame_slider1;
  GtkWidget **currPosArray;
  GtkWidget **sliderArray;
  GtkWidget *sliderVelocity;
  GtkWidget** po;
  guint *entry_id;

  static bool display_cartesian_pose(cartesianMover *currentPart);
  static bool display_axis_pose(cartesianMover *currentPart);
  static void position_slider_changed(GtkRange *range, cartesianMover *currentPart);
  //table functions
  void init_cartesian_table();
  static void cartesian_table_open(GtkButton* button, cartesianMover *currentPart);
  static void go_click(GtkButton* b, cartesianMover* cm);
  //GtkTreeModel* refresh_cartesian_position_list_model(double *TIM, int *SEQ, double **POS_LIST);
  //void add_cartesian_columns (GtkTreeView *tree_view, cartesianMover* cm);
  //table variables
 public:
  static void edited_cartesian_sequence (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, cartesianMover *cm);
  static void edited_cartesian_timing (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, cartesianMover *cm);
  static void cartesian_line_click(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, cartesianMover *cm);
  static void sequence_save(GtkButton *button,  cartesianMover* cm);
  static void sequence_load(GtkFileChooser *button, cartesianMover *cm );
  static void sequence_cycle_time(GtkButton*, cartesianMover*);
  static bool sequence_iterator_time(cartesianMover* cm);
  static void sequence_stop(GtkButton *button,cartesianMover* cm);
  void save_to_file(char* filenameIn, cartesianMover* cm);
  void load_from_file(char* filenameIn, cartesianMover* cm);

  GtkWidget *buttonGo;
  GtkWidget *buttonSave;
  GtkWidget *buttonLoad;
  GtkWidget *buttonCycTim;
  GtkWidget *buttonStp;

  int *SEQUENCE;
  int *INV_SEQUENCE;
  double *TIMING;
  double **STORED_POS;
  GtkWidget *treeview;
  guint32* timeout_seqeunce_rate;
  guint* timeout_seqeunce_id;
  int *SEQUENCE_ITERATOR;

 public:
  ResourceFinder *finder;

  char *partLabel;
  bool interfaceError;
  cartesianMover(GtkWidget *vbox_d, PolyDriver *partDd_d, char * labelPart, ResourceFinder *fnd);
  ~cartesianMover();
  void releaseDriver();

};

struct gtkClassCartesianData
{
  int *indexPointer;
  cartesianMover* cartesianPointer;
};

#endif
