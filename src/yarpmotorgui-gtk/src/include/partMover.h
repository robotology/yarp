#ifndef PARTMOVER_H
#define PARTMOVER_H

#include <math.h>
///////////YARP//////////
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/ResourceFinder.h>
///////////GTK///////////
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "gtkMessages.h"

// #include <yarp/String.h>
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

struct gtkClassData;

class partMover{
 private:
  GtkWidget *vbox;
  GtkWidget *button;
  PolyDriver *partDd;

  IPositionControl *pos;
  IEncoders *iencs;
  IAmplifierControl *amp;
  IPidControl *pid;
  ITorqueControl *trq;
  IImpedanceControl *imp;
  IControlLimits *lim;
  IControlCalibration2 *cal;
  IControlMode *ctrlmode;

  bool *CURRENT_POS_UPDATE;
  int *SEQUENCE_ITERATOR;
  double **STORED_POS;
  double **STORED_VEL;
  int     *SEQUENCE;
  int *INV_SEQUENCE;
  double    *TIMING;
  int *index;

  GtkWidget **sliderVelArray;
  GtkWidget **sliderArray;
  GtkWidget **currPosArray;
  GtkWidget **inPosArray;
  GtkWidget **frameColorBack;
  GtkWidget **frame_slider1;
  GtkWidget **frame_slider2;
  GtkWidget **framesArray;

  guint   *timeout_seqeunce_id;
  guint32 *timeout_seqeunce_rate;

  GtkWidget *frame1;
  
  guint *entry_id;

  //GtkWidget *button0;
  //GtkWidget *button9;
  //GtkWidget *button10;

  /*
   * If the position slider is realeased
   * execute the corresponding movement
   */
  static void run_click(GtkButton *button, gtkClassData* currentClassData);
  static void slider_release(GtkRange *range, gtkClassData* currentClassData);
  static void slider_pick(GtkRange *range,GdkEvent *e, gtkClassData* currentClassData);
  static void slider_unpick(GtkRange *range,GdkEvent *e, gtkClassData* currentClassData);
  static bool entry_update(partMover *currentPart);
  static void sliderVel_release(GtkRange *range, gtkClassData* currentClassData);
  static void dis_click(GtkButton *button, gtkClassData* currentClassData);
  static void calib_click(GtkButton *button, gtkClassData* currentClassData);
  static void home_click(GtkButton *button, gtkClassData* currentClassData);
  static void line_click(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gtkClassData* currentClassData);
  static GtkTreeModel * refresh_position_list_model(partMover* currentPartMover);
  static void add_columns (GtkTreeView *treeview, gtkClassData* currentClassData);
  static void edited_sequence (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, gtkClassData* currentClassData);
  static void edited_timing (GtkCellRendererText *cell, GtkTreePath *path_str, gchar *new_text, gtkClassData* currentClassData);
  static bool sequence_iterator(partMover* currentPart);
  static bool sequence_iterator_time(partMover* currentPart);
  static int get_index_selection(partMover* currentPart);
  static void pid_click(GtkButton *button, gtkClassData* currentClassData);
  static void pid_torque_click(GtkButton *button, gtkClassData* currentClassData);
  static void control_mode_click(GtkButton *button, GdkEventButton *event, gtkClassData* currentClassData);
  static void table_open(GtkButton *button, gtkClassData* currentClassData);

 public:
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *button4;
  GtkWidget *button5;
  GtkWidget *button6;
  GtkWidget *button7;
  GtkWidget *button8;
  ResourceFinder *finder;

  char *partLabel;
  bool interfaceError;
  GtkTreeModel *model;
  GtkWidget *treeview;
  static void enable_entry_update(partMover* currentPartMover);
  static void disable_entry_update(partMover* currentPartMover);
  partMover(GtkWidget *vbox_d, PolyDriver *partDd_d, char * labelPart, ResourceFinder *fnd);
  ~partMover();
  void releaseDriver();

  static void fixed_time_move(const double *cmdPositions, double cmdTime, partMover* currentPart);
  static void save_to_file(char* filename, partMover* currentPart);
  static void load_from_file(char* filename, partMover* currentPart);
  static void go_click(GtkButton *button,       partMover* currentPart);
  static void sequence_click(GtkButton *button, partMover* currentPart);
  static void sequence_time(GtkButton *button, partMover* currentPart);
  static void sequence_save(GtkButton *button,  partMover* currentPart);
  static void sequence_load(GtkFileChooser *button,  partMover* currentPart);
  static void sequence_cycle(GtkButton *button, partMover* currentPart);
  static void sequence_cycle_time(GtkButton *button, partMover* currentPart);
  static void sequence_stop(GtkButton *button,  partMover* currentPart);
  //static void sequence_stop_time(GtkButton *button,  partMover* currentPart);
  static void run_all(GtkButton *button, partMover* currentPart);
  static void calib_all(GtkButton *button, partMover* currentPart);
  static void home_all(GtkButton *button, partMover* currentPart);

  inline IPositionControl* get_IPositionControl () {return pos;}
  inline IEncoders * get_IEncoders () {return iencs;}
  inline IAmplifierControl* get_IAmplifierControl () {return amp;}
  inline IPidControl* get_IPidControl () {return pid;}
  inline ITorqueControl* get_ITorqueControl () {return trq;}
  inline IImpedanceControl* get_IImpedanceControl () {return imp;}
  inline IControlMode* get_IControlMode () {return ctrlmode;}
};

struct gtkClassData
{
  int *indexPointer;
  partMover* partPointer;
};

//Control Modes
#define MODE_IDLE					0x00
#define MODE_POSITION 				0x01
#define MODE_VELOCITY				0x02
#define MODE_TORQUE					0x03
#define MODE_IMPEDANCE_POS			0x04
#define MODE_IMPEDANCE_VEL			0x05
#define MODE_CALIB_ABS_POS_SENS		0x10
#define MODE_CALIB_HARD_STOPS		0x20
#define MODE_HANDLE_HARD_STOPS		0x30
#define MODE_MARGIN_REACHED    		0x40

#endif
