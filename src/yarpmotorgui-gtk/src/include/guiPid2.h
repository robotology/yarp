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


#ifndef GUIPIDTRQ_H
#define GUIPIDTRQ_H

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
//#include <CanBusInterface.h>
#include <iCub/DebugInterfaces.h>
#include "partMover.h"

namespace guiPid2
{
    static GtkWidget *trq_winPid = NULL;
    static GtkWidget *note_book = NULL;
    static GtkWidget *note_lbl1 = NULL;
    static GtkWidget *note_lbl2 = NULL;
    static GtkWidget *note_lbl3 = NULL;
    static GtkWidget *note_lbl4 = NULL;
    static GtkWidget *note_lbl5 = NULL;
    static GtkWidget *note_lbl6 = NULL;
    static GtkWidget *note_pag1 = NULL;
    static GtkWidget *note_pag2 = NULL;
    static GtkWidget *note_pag3 = NULL;
    static GtkWidget *note_pag4 = NULL;
    static GtkWidget *note_pag5 = NULL;
    static GtkWidget *note_pag6 = NULL;

    static GtkWidget *dbg_debugBaseEntry =  NULL;
    static GtkWidget *dbg_debug0Des = NULL;
    static GtkWidget *dbg_debug1Des = NULL;
    static GtkWidget *dbg_debug2Des = NULL;
    static GtkWidget *dbg_debug3Des = NULL;
    static GtkWidget *dbg_debug4Des = NULL;
    static GtkWidget *dbg_debug5Des = NULL;
    static GtkWidget *dbg_debug6Des = NULL;
    static GtkWidget *dbg_debug7Des = NULL;
    static GtkWidget *dbg_debug0Entry = NULL;
    static GtkWidget *dbg_debug1Entry = NULL;
    static GtkWidget *dbg_debug2Entry = NULL;
    static GtkWidget *dbg_debug3Entry = NULL;
    static GtkWidget *dbg_debug4Entry = NULL;
    static GtkWidget *dbg_debug5Entry = NULL;
    static GtkWidget *dbg_debug6Entry = NULL;
    static GtkWidget *dbg_debug7Entry = NULL;

    static GtkWidget *opl_koDes = NULL;
    static GtkWidget *opl_koEntry = NULL;

    static GtkWidget *trq_kpDes = NULL;
    static GtkWidget *trq_kdDes = NULL;
    static GtkWidget *trq_kiDes = NULL;
    static GtkWidget *trq_kffDes = NULL;
    static GtkWidget *trq_kbemfDes = NULL;
    static GtkWidget *trq_scaleDes = NULL;
    static GtkWidget *trq_offsetDes = NULL;
    static GtkWidget *trq_upStictionDes = NULL;
    static GtkWidget *trq_downStictionDes = NULL;
    static GtkWidget *trq_PWM_limitDes = NULL;
    static GtkWidget *trq_INT_limitDes = NULL;
    static GtkWidget *trq_kpEntry = NULL;
    static GtkWidget *trq_kdEntry = NULL;
    static GtkWidget *trq_kiEntry = NULL;
    static GtkWidget *trq_kffEntry = NULL;
    static GtkWidget *trq_kbemfEntry = NULL;
    static GtkWidget *trq_scaleEntry = NULL;
    static GtkWidget *trq_offsetEntry = NULL;
    static GtkWidget *trq_upStictionEntry = NULL;
    static GtkWidget *trq_downStictionEntry = NULL;
    static GtkWidget *trq_PWM_limitEntry = NULL;
    static GtkWidget *trq_INT_limitEntry = NULL;

    static GtkWidget *pos_kpDes = NULL;
    static GtkWidget *pos_kdDes = NULL;
    static GtkWidget *pos_kiDes = NULL;
    static GtkWidget *pos_scaleDes = NULL;
    static GtkWidget *pos_offsetDes = NULL;
    static GtkWidget *pos_posStictionDes = NULL;
    static GtkWidget *pos_negStictionDes = NULL;
    static GtkWidget *pos_PWM_limitDes = NULL;
    static GtkWidget *pos_INT_limitDes = NULL;
    static GtkWidget *pos_kpEntry = NULL;
    static GtkWidget *pos_kdEntry = NULL;
    static GtkWidget *pos_kiEntry = NULL;
    static GtkWidget *pos_scaleEntry = NULL;
    static GtkWidget *pos_offsetEntry = NULL;
    static GtkWidget *pos_posStictionEntry = NULL;
    static GtkWidget *pos_negStictionEntry = NULL;
    static GtkWidget *pos_PWM_limitEntry = NULL;
    static GtkWidget *pos_INT_limitEntry = NULL;

    static GtkWidget *imp_stiffDes = NULL;
    static GtkWidget *imp_dampDes = NULL;
    static GtkWidget *imp_offDes = NULL;
    static GtkWidget *imp_stiffMax = NULL;
    static GtkWidget *imp_dampMax = NULL;
    static GtkWidget *imp_stiffMin = NULL;
    static GtkWidget *imp_dampMin = NULL;
    static GtkWidget *imp_offMax = NULL;
    static GtkWidget *imp_offMin = NULL;

    static GtkWidget *imp_stiffEntry = NULL;
    static GtkWidget *imp_dampEntry = NULL;
    static GtkWidget *imp_offEntry = NULL;

    static int               *joint = NULL;
    static partMover         *currentPart = NULL;
    static IPidControl       *iPid = NULL;
    static IOpenLoopControl  *iOpl = NULL;
    static ITorqueControl    *iTrq = NULL;
    static IImpedanceControl *iImp = NULL;
    static IDebugInterface   *iDbg = NULL;
    static IVelocityControl  *iVel = NULL;
    //static ICanBus           *iCan = NULL;


    void guiPid2(void *button, void* data);
    void destroy_win (GtkButton *button, GtkWidget *window);
    void change_page  (GtkNotebook *notebook, GtkWidget   *page,  guint page_num,  gpointer user_data);
    void send_pos_pid (GtkButton *button, Pid *pid);
    void send_trq_pid (GtkButton *button, Pid *pid);
    void send_imp_pid (GtkButton *button, Pid *pid);
    void send_dbg_pid (GtkButton *button, Pid *pid);
    void send_opl_pid (GtkButton *button, Pid *pid);
    void dbg_debugBaseEntry_callback (GtkEntry *entry, gpointer  user_data);
    void receive_dbg_pid ();
    void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label, bool small = false);
    void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label, bool small = false);
    void displayPidValue(double k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label, bool small = false);
    void changePidValue(double k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label, bool small = false);
};

#endif
