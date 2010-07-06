#ifndef GUIPIDTRQ_H
#define GUIPIDTRQ_H

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "partMover.h"

namespace guiPidTrq
{
	static GtkWidget *trq_winPid = NULL;
	static GtkWidget *trq_kpDes = NULL;
	static GtkWidget *trq_kdDes = NULL;
	static GtkWidget *trq_kiDes = NULL;
	static GtkWidget *trq_scaleDes = NULL;
	static GtkWidget *trq_offsetDes = NULL;
	static GtkWidget *imp_stiffDes = NULL;
	static GtkWidget *imp_dampDes = NULL;

	static GtkWidget *trq_kpEntry = NULL;
	static GtkWidget *trq_kdEntry = NULL;
	static GtkWidget *trq_kiEntry = NULL;
	static GtkWidget *trq_scaleEntry = NULL;
	static GtkWidget *trq_offsetEntry = NULL;
	static GtkWidget *imp_stiffEntry = NULL;
	static GtkWidget *imp_dampEntry = NULL;

	static int * joint = NULL;
	static ITorqueControl *iTrq = NULL;
	static IImpedanceControl *iImp = NULL;

	void guiPidTrq(void *button, void* data);
	void destroy_main (GtkWindow *window,	gpointer   user_data);
	void destroy_win (GtkButton *button, GtkWindow *window);
	void send_pid (GtkButton *button, Pid *pid);
	void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label);
	void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label);
};

#endif
