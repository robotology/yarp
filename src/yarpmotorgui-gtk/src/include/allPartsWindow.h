#include "partMover.h"
#include "cartesianMover.h"
#include "robotMotorGui.h"

extern GtkWidget *buttonGoAll;
extern GtkWidget *buttonSeqAll;
extern GtkWidget *buttonSeqAllTime;
extern GtkWidget *buttonSeqAllSave;
extern GtkWidget *buttonSeqAllLoad;
extern GtkWidget *buttonSeqAllCycle;
extern GtkWidget *buttonSeqAllCycleTime;
extern GtkWidget *buttonSeqAllStop;
extern GtkWidget *buttonSeqAllStopTime;
extern GtkWidget *buttonRunAllParts;
extern GtkWidget *buttonHomeAllParts;

extern GtkWidget *buttonCrtSeqAllSave;
extern GtkWidget *buttonCrtSeqAllLoad;
extern GtkWidget *buttonCrtSeqAllCycleTime;
extern GtkWidget *buttonCrtSeqAllStop;

void run_all_parts (GtkButton *button, partMover** currentPartMover);
void home_all_parts (GtkButton *button, partMover** currentPartMover);
void go_all_click (GtkButton *button, partMover** currentPartMover);
void sequence_all_click_time (GtkButton *button, partMover** currentPartMover);
void sequence_all_click (GtkButton *button, partMover** currentPartMover);
void sequence_all_save (GtkButton *button, partMover** currentPartMover);
void sequence_all_load (GtkButton *button, partMover** currentPartMover);
void sequence_all_cycle (GtkButton *button, partMover** currentPartMover);
void sequence_all_cycle_time (GtkButton *button, partMover** currentPartMover);
void sequence_all_stop (GtkButton *button, partMover** currentPartMover);

void sequence_crt_all_save (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_load (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_cycle_time (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_stop (GtkButton *button, cartesianMover** currentPartMover);
