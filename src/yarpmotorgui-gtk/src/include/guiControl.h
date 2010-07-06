#ifndef GUICONTROL_H
#define GUICONTROL_H

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include "partMover.h"

namespace guiControl
{
	static GtkWidget *pos_winPid = NULL;
	static GtkWidget *label_title = NULL;
	static GtkWidget *radiobutton_mode_idl = NULL;
	static GtkWidget *radiobutton_mode_pos = NULL;
	static GtkWidget *radiobutton_mode_vel = NULL;
	static GtkWidget *radiobutton_mode_trq = NULL;
	static GtkWidget *radiobutton_mode_imp_pos = NULL;
	static GtkWidget *radiobutton_mode_imp_vel = NULL;
	static GtkWidget *radiobutton_mode_open = NULL;

	static int * joint = NULL;
	static IControlMode *icntrl = NULL;
    static IAmplifierControl *iamp = NULL;
    static IPidControl *ipid = NULL;

	void guiControl(void *button, void* data);
	void destroy_main (GtkWindow *window,	gpointer   user_data);
	void destroy_win (GtkButton *button, GtkWindow *window);
	void displayPidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char *label);
	void changePidValue(int k, GtkWidget *inv,GtkWidget *entry, int posX, int posY, const char * label);
	void update_menu(int control_mode);

	static void radio_click_idl (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_pos (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_vel (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_trq (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_imp_pos (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_imp_vel (GtkWidget* radio , gtkClassData* currentClassData);
	static void radio_click_open(GtkWidget* radio , gtkClassData* currentClassData);	
};

#endif
