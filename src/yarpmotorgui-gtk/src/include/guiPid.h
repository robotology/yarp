#ifndef GUIPID_H
#define GUIPID_H

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "partMover.h"

namespace guiPid
{
	static GtkWidget *pos_winPid = NULL;
	static GtkWidget *pos_kpDes = NULL;
	static GtkWidget *pos_kdDes = NULL;
	static GtkWidget *pos_kiDes = NULL;
	static GtkWidget *pos_scaleDes = NULL;
	static GtkWidget *pos_offsetDes = NULL;

	static GtkWidget *pos_kpEntry = NULL;
	static GtkWidget *pos_kdEntry = NULL;
	static GtkWidget *pos_kiEntry = NULL;
	static GtkWidget *pos_scaleEntry = NULL;
	static GtkWidget *pos_offsetEntry = NULL;

	static int * joint = NULL;
	static IPidControl *ipid = NULL;

	void guiPid(void *button, void* data);
	void destroy_main (GtkWindow *window,	gpointer   user_data);
	void destroy_win (GtkButton *button, GtkWindow *window);
	void send_pid (GtkButton *button, Pid *pid);
	void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label);
	void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label);
};

#endif
