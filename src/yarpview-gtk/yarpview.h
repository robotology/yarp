/*
 * Copyright (C) 2006 Matteo Brunettini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if !defined(GCAMVIEW_H)
#define GCAMVIEW_H

//=============================================================================
// YARP Includes - Class Specific
//=============================================================================
#include <yarp/sig/Image.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
//=============================================================================
// GTK Includes
//=============================================================================
#include <gtk/gtk.h>
//=============================================================================
// Local Includes
//=============================================================================
#include "ImagePort.h"
#include "YarpImage2Pixbuf.h"
#include "ViewerResources.h"

//-------------------------------------------------
// Callbacks
//-------------------------------------------------
gboolean forceDraw(gpointer data);
// Timeout CB
gint timeout_CB (gpointer data);
// Main Window close CB
gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );
//  Window redraw CB
gint expose_CB (GtkWidget *widget, GdkEventExpose *event, gpointer data);
// Click on Drawinf Area CB
gint clickDA_CB (GtkWidget *widget, GdkEventButton *event, gpointer data);
// Menubar CBs
gint menuFileQuit_CB(GtkWidget *widget, gpointer data);
gint menuHelpAbout_CB(GtkWidget *widget, gpointer data);
gint menuImageSize_CB(GtkWidget *widget, gpointer data);
gint menuImageRatio_CB(GtkWidget *widget, gpointer data);
gint menuImageInterval_CB(GtkWidget *widget, gpointer data);
gint menuImageFramerate_CB(GtkWidget *widget, gpointer data);
gint menuFileSingle_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gint menuFileSet_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gint menuImageFreeze_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
// Dialogs CBs
gint saveSingleDelete_CB(GtkWidget *widget, gpointer data);
gint saveSetDelete_CB(GtkWidget *widget, gpointer data);
gint saveSingleClicked_CB(GtkWidget *widget, gpointer data);
gint saveSetStartClicked_CB(GtkWidget *widget, gpointer data);
gint saveSetStopClicked_CB(GtkWidget *widget, gpointer data);

void setTimedMode(guint dT);

void setSynchroMode();

void saveImageNow();

//-------------------------------------------------
// Non Modal Dialogs
//-------------------------------------------------
GtkWidget* createSaveSingleDialog();
GtkWidget* createSaveSetDialog();
//-------------------------------------------------
// Main Window Menubar
//-------------------------------------------------
GtkWidget* createMenubar();
//-------------------------------------------------
// Main Window Statusbar
//-------------------------------------------------
void updateStatusbar(GtkWidget *statusbar, gchar *msg);
//-------------------------------------------------
// Main Window
//-------------------------------------------------
GtkWidget* createMainWindow(void);
//-------------------------------------------------
// Service Functions
//-------------------------------------------------
bool getImage();
void parseOptFile(char *fileName);
bool parseParameters(int argc, char *argv[]);
void saveOptFile(char *fileName);
void saveCurrentFrame();
void setOptionsToDefault();
bool openPorts();
void closePorts();
bool setUp();
void cleanExit();

void printHelp();

//-------------------------------------------------
// Global Variables
//-------------------------------------------------
// main window


//-------------------------------------------------
// Program Options
//-------------------------------------------------
struct mOptions
{
    unsigned int    refreshTime;
    char            portName[256];
    char            networkName[256];
    int             windWidth;
    int             windHeight;
    int             posX;
    int             posY;
    char            fileName[256];
    int             saveOnExit;
    char            outPortName[256];
    char            outNetworkName[256];
    int             outputEnabled;
    bool            synch;
    bool            compact;
    bool            keepabove;
};
typedef struct mOptions pgmOptions;



#endif // #if !defined(GCAMVIEW_H)
