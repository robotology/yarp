// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
#include "YARPImgRecv.h"

//-------------------------------------------------
// Callbacks
//-------------------------------------------------
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
void updateStatusbar (GtkStatusbar *statusbar);
//-------------------------------------------------
// Main Window
//-------------------------------------------------
GtkWidget* createMainWindow(void);
//-------------------------------------------------
// Service Functions
//-------------------------------------------------
bool getImage();
bool yarpImage2pixbuf(yarp::sig::ImageOf<yarp::sig::PixelRgb> *sourceImg, GdkPixbuf* destPixbuf);
void parseOptFile(char *fileName);
void parseParameters(int argc, char *argv[]);
void saveOptFile(char *fileName);
void saveCurrentFrame();
void setOptionsToDefault();
bool openPorts();
void closePorts();
void setUp();
void cleanExit();

//-------------------------------------------------
// Global Variables
//-------------------------------------------------
// main window 
static GtkWidget *mainWindow = NULL;
// Current frame
static GdkPixbuf *frame = NULL;
// Drawing Area
static GtkWidget *da;
// Non Modal Dialogs
static GtkWidget *saveSingleDialog, *saveSetDialog;
// MenuBar
static GtkWidget *menubar;
static GtkWidget *fileMenu, *imageMenu, *helpMenu;
static GtkWidget *fileItem, *imageItem, *helpItem;
static GtkWidget *fileSingleItem, *fileSetItem, *fileQuitItem;
static GtkWidget *imageSizeItem, *imageRatioItem, *imageFreezeItem, *imageFramerateItem, *imageIntervalItem;
static GtkWidget *helpAboutItem;
// StatusBar
static GtkWidget *statusbar;
// Timeout ID
static guint timeout_ID;
// Image Receiver
YARPImgRecv _imgRecv;
// Image to Display
yarp::sig::ImageOf<yarp::sig::PixelRgb> _inputImg;
// Semaphore
yarp::os::Semaphore _semaphore;
// Frame Number
int _frameN;
// Save Set of Images mode
bool _savingSet;
// Dimension of widgets on main screen
int _occupiedHeight;
// Frame must be freezed
bool _freezed;
// Output Point Port
//YARPOutputPortOf<YARPBottle> *_pOutPort;
// Output Bottle Container
//YARPBottle _outBottle;
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

pgmOptions _options;

#endif // #if !defined(GCAMVIEW_H)
