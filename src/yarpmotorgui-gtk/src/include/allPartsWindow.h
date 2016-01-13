/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Francesco Nori <francesco.nori@iit.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/

#ifndef YARP_YARPMOTORGUI_GTK_ALLPARTSMOVER_H
#define YARP_YARPMOTORGUI_GTK_ALLPARTSMOVER_H


#include "partMover.h"
#include "yarpmotorgui.h"

#ifdef CARTESIAN_MOVER
#include "cartesianMover.h"
#endif

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

#ifdef CARTESIAN_MOVER
void sequence_crt_all_save (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_load (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_cycle_time (GtkButton *button, cartesianMover** currentPartMover);
void sequence_crt_all_stop (GtkButton *button, cartesianMover** currentPartMover);
#endif

#endif // YARP_YARPMOTORGUI_GTK_GTKMESSAGES_H
