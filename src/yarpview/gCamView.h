// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Matteo Brunettini
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#if !defined(GCAMVIEW_H)
#define GCAMVIEW_H

//=============================================================================
// YARP Includes - General
//=============================================================================
//#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
//=============================================================================
// YARP Includes - Class Specific
//=============================================================================
#include <yarp/sig/Image.h>
//#include <yarp/YARPImageFile.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
//#include <yarp/YARPParseParameters.h>
//#include <yarp/YARPConfigFile.h>
//#include <yarp/YARPBottleContent.h>
//=============================================================================
// GTK Includes 
//=============================================================================
#include <gtk/gtk.h>
//=============================================================================
// Local Includes 
//=============================================================================
#include "ImagePort.h"

#include "yarpImage2Pixbuf.h"

#include "ViewerResources.h"

//-------------------------------------------------
// Callbacks
//-------------------------------------------------
gboolean forceDraw(gpointer data);
// Timeout CB
static gint timeout_CB (gpointer data);
// Main Window close CB
static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );
//  Window redraw CB
static gint expose_CB (GtkWidget *widget, GdkEventExpose *event, gpointer data);
// Click on Drawinf Area CB
static gint clickDA_CB (GtkWidget *widget, GdkEventButton *event, gpointer data);
// Menubar CBs
static gint menuFileQuit_CB(GtkWidget *widget, gpointer data);
static gint menuHelpAbout_CB(GtkWidget *widget, gpointer data);
static gint menuImageSize_CB(GtkWidget *widget, gpointer data);
static gint menuImageRatio_CB(GtkWidget *widget, gpointer data);
static gint menuImageInterval_CB(GtkWidget *widget, gpointer data);
static gint menuImageFramerate_CB(GtkWidget *widget, gpointer data);
static gint menuFileSingle_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gint menuFileSet_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
static gint menuImageFreeze_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data);
// Dialogs CBs
static gint saveSingleDelete_CB(GtkWidget *widget, gpointer data);
static gint saveSetDelete_CB(GtkWidget *widget, gpointer data);
static gint saveSingleClicked_CB(GtkWidget *widget, gpointer data);
static gint saveSetStartClicked_CB(GtkWidget *widget, gpointer data);
static gint saveSetStopClicked_CB(GtkWidget *widget, gpointer data);
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
void parseParameters(int argc, char *argv[]);
void saveOptFile(char *fileName);
void saveCurrentFrame();
void setOptionsToDefault();
bool openPorts();
void closePorts();
bool setUp();
void cleanExit();

//-------------------------------------------------
// Global Variables
//-------------------------------------------------
// main window 
static    GtkWidget *saveSingleDialog;
static    GtkWidget *saveSetDialog;
static    GtkWidget *menubar;
static    GtkWidget *fileMenu, *imageMenu, *helpMenu;
static    GtkWidget *fileItem, *imageItem, *helpItem;
static    GtkWidget *fileSingleItem, *fileSetItem, *fileQuitItem;
static    GtkWidget *imageSizeItem, *imageRatioItem, *imageFreezeItem, *imageFramerateItem, *imageIntervalItem;
static    GtkWidget *helpAboutItem;
// StatusBar
static    GtkWidget *statusbar;
static    GtkWidget *fpsStatusBar;
static    GtkWidget *fpsStatusBar2;

static    guint timeout_ID;
static    guint timeout_update_ID;

// static ViewerWidgets _widgets;
static ViewerResources _resources;


//-------------------------------------------------
// Program Options 
//-------------------------------------------------
struct mOptions
{
	unsigned int	refreshTime;
	char			portName[256];
	char			networkName[256];
	int				windWidth;
	int				windHeight;
	int				posX;
	int				posY;
	char			fileName[256];
	int				saveOnExit;
	int				logpolar;
	int				fovea;
	char			outPortName[256];
	char			outNetworkName[256];
	int				outputEnabled;
};
typedef struct mOptions pgmOptions;



#endif // #if !defined(GCAMVIEW_H)
