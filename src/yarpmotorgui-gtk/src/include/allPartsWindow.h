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

#include "partMover.h"
#include "robotMotorGui.h"

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