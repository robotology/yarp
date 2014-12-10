// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 Matteo Brunettini, Lorenzo Natale
 * Lorenzo Natale, 2008. Added multithread support, cleaned code, added fps count
 * and synchronous display.
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// gCamView.cpp : Defines the entry point for the application.
//

#include <yarp/os/impl/PlatformStdio.h>

#include "yarpview.h"
#include "FpsStats.h"

#include <yarp/os/Property.h>
#include <yarp/os/Network.h>

#include "YarpImage2Pixbuf.h"
#include "ImagePort.h"

#include <yarp/os/BufferedPort.h>

#include <string>
#include <sstream>

#if defined(__GNUG__) && defined(__GNUC__) && __GNUC__ >= 4
# ifdef NULL
#  undef NULL
# endif
# define NULL __null
#endif

using namespace yarp::os;

GtkWidget *mainWindow=0;

BufferedPort<yarp::sig::FlexImage> *ptr_inputPort=0;
InputCallback *ptr_portCallback=0;

FpsStats displayFpsData;
FpsStats portFpsData;

// Frame Number
int _frameN;
// Save Set of Images mode
bool _savingSet;
// Output Point Port
yarp::os::BufferedPort<yarp::os::Bottle> *_pOutPort=0;
// Output Bottle Container
yarp::os::Bottle _outBottle;

pgmOptions _options;

GtkWidget *saveSingleDialog;
GtkWidget *saveSetDialog;
GtkWidget *menubar;
GtkWidget *fileMenu, *imageMenu, *helpMenu;
GtkWidget *fileItem, *imageItem, *helpItem;
GtkWidget *fileSingleItem, *fileSetItem, *fileQuitItem;
GtkWidget *imageSizeItem, *imageRatioItem, *imageFreezeItem, *imageFramerateItem;
GtkWidget *synchroDisplayItem;
GtkWidget *helpAboutItem;
// StatusBar
GtkWidget *statusbar;
GtkWidget *fpsStatusBar;
GtkWidget *fpsStatusBar2;

guint timeout_ID=0;
guint timeout_update_ID=0;

// static ViewerWidgets _widgets;
ViewerResources _resources;

static void createObjects() {
    ptr_inputPort = new BufferedPort<yarp::sig::FlexImage>;
    ptr_portCallback = new InputCallback;
}

static void deleteObjects() {
    if (ptr_inputPort!=0)
        delete ptr_inputPort;
    if (ptr_portCallback!=0)
        delete ptr_portCallback;
}

inline void periodToFreq(double avT, double mT, double MT, double &avH, double &mH, double &MH)
{
//    avH=1000*avT;
//    mH=1000*mT;
//   MH=1000*MT;

    if (avT!=0)
        avH=1.0/avT;
    else
        avH=0;

    if (mT!=0)
        MH=1.0/mT;
    else
        MH=0;

    if (MT!=0)
        mH=1.0/MT;
    else
        mH=0;
}


void saveImageNow()
{
    if (_savingSet) {
        saveCurrentFrame();
    }
}


//-------------------------------------------------
// Main Window Callbacks
//-------------------------------------------------

gboolean forceDraw(gpointer data)
{
    gdk_threads_enter();
    _resources.invalidateDrawArea();
    gdk_threads_leave();

    return FALSE; //removed from the queue if returning false
}

gint timeout_update_CB(gpointer data)
{
    double av, max, min;
    portFpsData.getStats(av, min, max);
    portFpsData.reset();

    gchar *msg;

    double avHz;
    double minHz;
    double maxHz;

    periodToFreq(av, min, max, avHz, minHz, maxHz);

    gdk_threads_enter();

    if (_options.compact == 0)
    {
        msg=g_strdup_printf("Port: %.1f (min:%.1f max:%.1f) fps", avHz, minHz, maxHz);
        updateStatusbar(fpsStatusBar, msg);
        g_free(msg);
    }

    displayFpsData.getStats(av, min, max);
    displayFpsData.reset();

    periodToFreq(av, min, max, avHz, minHz, maxHz);
    if (_options.compact == 0)
    {
        msg=g_strdup_printf("Display: %.1f (min:%.1f max:%.1f) fps", avHz, minHz, maxHz);
        updateStatusbar(fpsStatusBar2, msg);
        g_free(msg);
    }

    gdk_threads_leave();

    return TRUE;
}

gint timeout_CB (gpointer data)
{
    gdk_threads_enter();
    _resources.invalidateDrawArea();
    gdk_threads_leave();
    return TRUE;
}


gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{
    // If you return FALSE in the "delete_event" signal handler,
    // GTK will emit the "destroy" signal. Returning TRUE means
    // you don't want the window to be destroyed.
    // This is useful for popping up 'are you sure you want to quit?'
    // type dialogs.

    cleanExit();

    return TRUE;

}

gint expose_CB (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if (mainWindow)
    {
        _resources.draw(widget,
                        event->area.x,
                        event->area.y,
                        event->area.width,
                        event->area.height,
                        true);
    }

    return TRUE;
}

static gboolean
configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
    _resources.configure(widget,
                         widget->allocation.width,
                         widget->allocation.height);

  return TRUE;
}
gint menuFileQuit_CB(GtkWidget *widget, gpointer data)
{
    cleanExit();

    return TRUE;
}

gint menuHelpAbout_CB(GtkWidget *widget, gpointer data)
{
#if GTK_CHECK_VERSION(2,6,0)
    const gchar *authors[] =
    {
        "Yarp developers",
        NULL
    };
    const gchar *license =
        "Released under the terms of the LGPLv2.1 or later, see LGPL.TXT\n"
        "The complete license description is contained in the\n"
        "COPYING file included in this distribution.\n"
        "Please refer to this file for complete\n"
        "information about the licensing of YARP.\n"
        "\n"
        "DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE\n"
        "SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS\n"
        "DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS\n"
        "EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE\n"
        "SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT\n"
        "OWNERS AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED\n"
        "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        "FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO\n"
        "EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE\n"
        "LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN\n"
        "ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN\n"
        "ONNECTION WITH THE SOFTWARE.\n";

    gtk_show_about_dialog(GTK_WINDOW(mainWindow),
                          "name", "yarpview",
                          "version", "1.1",
                          "license", license,
                          "website", "http://sourceforge.net/projects/yarp0",
                          "comments", "Program to display images received on a port.",
                          "authors", authors,
                          NULL);
#else
    printf("Missing functionality on older GTK version, sorry\n");
#endif

    return TRUE;
}


gint menuImageSize_CB(GtkWidget *widget, gpointer data)
{
    int targetWidth, targetHeight;
    targetWidth = _resources.imageWidth();
    targetHeight = _resources.imageHeight();
    if (targetWidth!=0&&targetHeight!=0) {
        unsigned int windowH=mainWindow->allocation.height;
        unsigned int windowW=mainWindow->allocation.width;
        unsigned int daH=_resources.drawArea->allocation.height;
        unsigned int daW=_resources.drawArea->allocation.width;

        //trick: we compute the new size of the window by difference

        unsigned int newHeight=(windowH-daH)+targetHeight;
        unsigned int newWidth=(windowW-daW)+targetWidth;
        gtk_window_resize(GTK_WINDOW(mainWindow), newWidth, newHeight);
    }
    return TRUE;
}

gint menuImageRatio_CB(GtkWidget *widget, gpointer data)
{
    double ratio;
    int imgWidth, imgHeight;
    int targetWidth, targetHeight;
    imgWidth = _resources.imageWidth();
    imgHeight = _resources.imageHeight();
    if (imgWidth!=0&&imgHeight!=0) {
        unsigned int windowH=mainWindow->allocation.height;
        unsigned int windowW=mainWindow->allocation.width;

        unsigned int daW = _resources.drawArea->allocation.width;
        unsigned int daH = _resources.drawArea->allocation.height;
        ratio = double(imgWidth) / double(imgHeight);
        targetWidth = int(double(daH) * ratio);
        targetHeight = daH;

        //trick: we compute the new size of the window by difference
        unsigned int newHeight=(windowH-daH)+targetHeight;
        unsigned int newWidth=(windowW-daW)+targetWidth;
        gtk_window_resize(GTK_WINDOW(mainWindow), newWidth, newHeight);
    }
    return TRUE;
}

gint menuFileSingle_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) )
    {
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSetItem), FALSE);
        gtk_widget_show_all (saveSingleDialog);
    }
    else
    {
        gtk_widget_hide (saveSingleDialog);
    }

    return TRUE;
}

gint menuFileSet_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
#if GTK_CHECK_VERSION(2,6,0)

    if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) )
    {
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSingleItem), FALSE);
        gtk_widget_show_all (saveSetDialog);
    }
    else
    {
        gtk_widget_hide (saveSetDialog);
    }

#endif

    return TRUE;
}

gint menuSynchroDisplay_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) )
    {
        setSynchroMode();
        _options.synch=true;
    }
    else
    {
        setTimedMode(_options.refreshTime);
        _options.synch=false;
    }
    return TRUE;
}


gint menuImageFreeze_CB(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if ( gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(widget)) )
    {
        _resources.freeze();
    }
    else
    {
        _resources.unfreeze();
    }
    return TRUE;
}

gint saveSingleDelete_CB (GtkWidget *widget, gpointer data)
{
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSingleItem), FALSE);

    return (TRUE);
}

gint saveSetDelete_CB (GtkWidget *widget, gpointer data)
{
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(fileSetItem), FALSE);

    return (TRUE);
}

gint saveSingleClicked_CB(GtkWidget *widget, gpointer data)
{
    saveCurrentFrame();

    return (TRUE);
}

gint saveSetStartClicked_CB(GtkWidget *widget, gpointer data)
{
    _savingSet = true;

    return (TRUE);
}

gint saveSetStopClicked_CB(GtkWidget *widget, gpointer data)
{
    _savingSet = false;

    return (TRUE);
}

gint menuImageFramerate_CB(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkWidget *hbox;
    GtkWidget *stock;

    GtkWidget *label;
    GtkWidget *spinner;
    GtkAdjustment *spinner_adj;
    gint response;

    dialog = gtk_dialog_new_with_buttons ("New Refresh Time",
                                          GTK_WINDOW (mainWindow),
                                          GTK_DIALOG_MODAL,
                                          GTK_STOCK_OK,
                                          GTK_RESPONSE_OK,
                                          GTK_STOCK_CANCEL,
                                          GTK_RESPONSE_CANCEL,
                                          NULL);

    hbox = gtk_hbox_new (FALSE, 8);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
    stock = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start (GTK_BOX (hbox), stock, FALSE, FALSE, 0);
    label = gtk_label_new_with_mnemonic ("Insert new refresh time (in mSec):");
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    spinner_adj = (GtkAdjustment *) gtk_adjustment_new (_options.refreshTime, 10.0, 1000.0, 1.0, 5.0, 5.0);
    spinner = gtk_spin_button_new (spinner_adj, 1.0, 0);
    gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 0);

    gtk_widget_show_all (hbox);

    response = gtk_dialog_run (GTK_DIALOG (dialog));

    if (response == GTK_RESPONSE_OK)
    {
        _options.refreshTime = (int) gtk_spin_button_get_value (GTK_SPIN_BUTTON(spinner));

        if (!_options.synch)
            setTimedMode(_options.refreshTime);

//        gchar *msg;
//        msg = g_strdup_printf ("%s",_options.portName);
//        updateStatusbar(statusbar,msg);
//        g_free(msg);
    }

    gtk_widget_destroy (dialog);

    return (TRUE);
}

gint clickDA_CB (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int imageX, imageY;
    int clickX, clickY;
    int daWidth, daHeight;
    int imageWidth, imageHeight;
    double ratioX, ratioY;

    imageWidth = _resources.imageWidth();
    imageHeight = _resources.imageHeight();

    if ( (imageWidth != 0) && (imageHeight != 0) ) {
        daWidth = widget->allocation.width;
        daHeight = widget->allocation.height;
        clickX = (int) event->x;
        clickY = (int) event->y;
        ratioX = double(clickX) / double(daWidth);
        ratioY = double(clickY) / double(daHeight);
        imageX = int(imageWidth * ratioX + 0.5);
        imageY = int(imageHeight * ratioY + 0.5);

        printf("Transmitting click information...\n");
        if (_pOutPort!=NULL) {
            yarp::os::Bottle& bot = _pOutPort->prepare();
            bot.clear();
            bot.addInt(imageX);
            bot.addInt(imageY);
            //_pOutPort->Content() = _outBottle;
            _pOutPort->write();
        }

    } else {
        printf("I would send a position, but there's no image for scaling\n");
    }

    return TRUE;
}



void setTimedMode(guint dT)
{
    ptr_portCallback->mustDraw(false);
    if (timeout_ID!=0)
        gtk_timeout_remove(timeout_ID);

    timeout_ID = gtk_timeout_add (dT, timeout_CB, NULL);
}

void setSynchroMode()
{
    gtk_timeout_remove(timeout_ID);
    timeout_ID=0;
    ptr_portCallback->mustDraw(true);
}

//-------------------------------------------------
// Non Modal Dialogs
//-------------------------------------------------
GtkWidget* createSaveSingleDialog(void)
{

    GtkWidget *dialog = NULL;
    GtkWidget *button;
    GtkWidget *hbox;
    dialog = gtk_dialog_new ();
    gtk_window_set_title(GTK_WINDOW(dialog), "Save Snapshot");
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWindow));
    //gtk_window_resize(GTK_WINDOW(dialog), 185, 40);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    //gtk_window_set_default_size(GTK_WINDOW(dialog), 185, 40);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
    gtk_dialog_set_has_separator (GTK_DIALOG(dialog), FALSE);
    hbox = gtk_hbox_new (TRUE, 8); // parameters (gboolean homogeneous_space, gint spacing);
    button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_widget_set_size_request (GTK_WIDGET(button), 150,50);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 16); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (saveSingleClicked_CB), NULL);
    gtk_signal_connect (GTK_OBJECT (dialog), "delete_event", GTK_SIGNAL_FUNC (saveSingleDelete_CB), NULL);

    //gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

    return dialog;
}

GtkWidget* createSaveSetDialog(void)
{
    GtkWidget *dialog = NULL;
    GtkWidget *saveButton;
    GtkWidget *stopButton;
    GtkWidget *hbox;
    dialog = gtk_dialog_new ();
    gtk_window_set_title(GTK_WINDOW(dialog), "Save Image Set");
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(mainWindow));
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    //gtk_window_set_default_size(GTK_WINDOW(dialog), 190, 40);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
    gtk_dialog_set_has_separator (GTK_DIALOG(dialog), FALSE);
#if GTK_CHECK_VERSION(2,6,0)
    saveButton = gtk_button_new_from_stock(GTK_STOCK_MEDIA_RECORD);
    stopButton = gtk_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
#else
    printf("Missing functionality on older GTK version, sorry\n");
#endif
    gtk_widget_set_size_request (GTK_WIDGET(saveButton), 80,50);
    gtk_widget_set_size_request (GTK_WIDGET(stopButton), 80,50);

    hbox = gtk_hbox_new (TRUE, 8); // parameters (gboolean homogeneous_space, gint spacing);
    gtk_box_pack_start (GTK_BOX (hbox), saveButton, TRUE, TRUE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_box_pack_start (GTK_BOX (hbox), stopButton, TRUE, TRUE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox, FALSE, FALSE, 8); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_signal_connect (GTK_OBJECT (saveButton), "clicked", GTK_SIGNAL_FUNC (saveSetStartClicked_CB), NULL);
    gtk_signal_connect (GTK_OBJECT (stopButton), "clicked", GTK_SIGNAL_FUNC (saveSetStopClicked_CB), NULL);
    gtk_signal_connect (GTK_OBJECT (dialog), "delete_event", GTK_SIGNAL_FUNC (saveSetDelete_CB), NULL);

    return dialog;
}

//-------------------------------------------------
// Main Window Menubar
//-------------------------------------------------
GtkWidget* createMenubar(void)
{
    GtkWidget *menubar;

    menubar =  gtk_menu_bar_new ();
    GtkWidget *menuSeparator;
    // Submenus Items on menubar
    fileItem = gtk_menu_item_new_with_label ("File");
    imageItem = gtk_menu_item_new_with_label ("Image");
    helpItem = gtk_menu_item_new_with_label ("Help");
    // Submenu: File
    fileMenu = gtk_menu_new();
    fileSingleItem = gtk_check_menu_item_new_with_label ("Save single image..");
    gtk_menu_append( GTK_MENU(fileMenu), fileSingleItem);
    gtk_signal_connect( GTK_OBJECT(fileSingleItem), "toggled", GTK_SIGNAL_FUNC(menuFileSingle_CB), mainWindow);
    fileSetItem = gtk_check_menu_item_new_with_label ("Save a set of images..");
    gtk_menu_append( GTK_MENU(fileMenu), fileSetItem);
    gtk_signal_connect( GTK_OBJECT(fileSetItem), "toggled", GTK_SIGNAL_FUNC(menuFileSet_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(fileMenu), menuSeparator);
    fileQuitItem = gtk_menu_item_new_with_label ("Quit");
    gtk_menu_append( GTK_MENU(fileMenu), fileQuitItem);
    gtk_signal_connect( GTK_OBJECT(fileQuitItem), "activate", GTK_SIGNAL_FUNC(menuFileQuit_CB), mainWindow);
    // Submenu: Image
    imageMenu = gtk_menu_new();
    imageSizeItem = gtk_menu_item_new_with_label ("Original size");
    gtk_menu_append( GTK_MENU(imageMenu), imageSizeItem);
    gtk_signal_connect( GTK_OBJECT(imageSizeItem), "activate", GTK_SIGNAL_FUNC(menuImageSize_CB), mainWindow);
    imageRatioItem = gtk_menu_item_new_with_label ("Original aspect ratio");
    gtk_menu_append( GTK_MENU(imageMenu), imageRatioItem);
    gtk_signal_connect( GTK_OBJECT(imageRatioItem), "activate", GTK_SIGNAL_FUNC(menuImageRatio_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(imageMenu), menuSeparator);
    imageFreezeItem = gtk_check_menu_item_new_with_label ("Freeze");
    gtk_menu_append( GTK_MENU(imageMenu), imageFreezeItem);
    gtk_signal_connect( GTK_OBJECT(imageFreezeItem), "toggled", GTK_SIGNAL_FUNC(menuImageFreeze_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(imageMenu), menuSeparator);
    synchroDisplayItem = gtk_check_menu_item_new_with_label ("Synch display");
    gtk_menu_append( GTK_MENU(imageMenu), synchroDisplayItem);
    gtk_signal_connect( GTK_OBJECT(synchroDisplayItem), "toggled", GTK_SIGNAL_FUNC(menuSynchroDisplay_CB), mainWindow);
    menuSeparator = gtk_separator_menu_item_new();
    gtk_menu_append( GTK_MENU(imageMenu), menuSeparator);
    imageFramerateItem = gtk_menu_item_new_with_label ("Change refresh interval..");
    gtk_menu_append( GTK_MENU(imageMenu), imageFramerateItem);
    gtk_signal_connect( GTK_OBJECT(imageFramerateItem), "activate", GTK_SIGNAL_FUNC(menuImageFramerate_CB), mainWindow);
    // Submenu: Help
    helpMenu = gtk_menu_new();
    helpAboutItem = gtk_menu_item_new_with_label ("About..");
    gtk_menu_append( GTK_MENU(helpMenu), helpAboutItem);
    gtk_signal_connect( GTK_OBJECT(helpAboutItem), "activate", GTK_SIGNAL_FUNC(menuHelpAbout_CB), mainWindow);
    // linking the submenus to items on menubar
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileItem), fileMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(imageItem), imageMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpItem), helpMenu);
    // appending the submenus to the menubar
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), fileItem);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), imageItem);
    gtk_menu_item_set_right_justified (GTK_MENU_ITEM (helpItem), TRUE);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), helpItem);

    return menubar;
}

//-------------------------------------------------
// Main Window Statusbar
//-------------------------------------------------
void updateStatusbar(GtkWidget *statusbar, gchar *msg)
{
    GtkStatusbar *sb=GTK_STATUSBAR (statusbar);

    gtk_statusbar_pop (sb, 0); // clear any previous message, underflow is allowed
    gtk_statusbar_push (sb, 0, msg);
}

//-------------------------------------------------
// Main Window
//-------------------------------------------------
GtkWidget* createMainWindow(void)
{
    GtkRequisition actualSize;
    GtkWidget* window;
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    if (_options.compact == 1)
    {
        gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
    }
    if (_options.keepabove == 1)
    {
        gtk_window_set_keep_above (GTK_WINDOW (window), TRUE);
    }

    gtk_window_set_title (GTK_WINDOW (window), "YARP GTK Image Viewer");
    gtk_window_set_default_size(GTK_WINDOW (window), _options.windWidth, _options.windHeight);
    gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
    // When the window is given the "delete_event" signal (this is given
    // by the window manager, usually by the "close" option, or on the
    // titlebar), we ask it to call the delete_event () function
    // as defined above. The data passed to the callback
    // function is NULL and is ignored in the callback function.
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);
    // Box for main window
    GtkWidget *box;
    box = gtk_vbox_new (FALSE, 0); // parameters (gboolean homogeneous_space, gint spacing);
    gtk_container_add (GTK_CONTAINER (window), box);
    // MenuBar for main window
    menubar = createMenubar();
    gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, TRUE, 0); // parameters (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
    gtk_widget_size_request(menubar, &actualSize);

    // Drawing Area : we draw the image here
    _resources.drawArea = gtk_drawing_area_new ();
    _resources.mainWindow=mainWindow;
    _resources.attach(&displayFpsData);

    g_signal_connect (GTK_OBJECT(_resources.drawArea),"configure_event",
              (GtkSignalFunc) configure_event, NULL);

    g_signal_connect (GTK_OBJECT(_resources.drawArea), "expose_event", G_CALLBACK (expose_CB), NULL);

    if (_options.outputEnabled == 1)
    {
        g_signal_connect (_resources.drawArea, "button_press_event", G_CALLBACK (clickDA_CB), NULL);
        // Ask to receive events the drawing area doesn't normally subscribe to
        gtk_widget_set_events (_resources.drawArea, gtk_widget_get_events (_resources.drawArea) | GDK_BUTTON_PRESS_MASK);
    }
    gtk_box_pack_start(GTK_BOX(box), _resources.drawArea, TRUE, TRUE, 0);
    // StatusBar for main window

    if (_options.compact == 0)
    {
        // StatusBar for main window

        fpsStatusBar=gtk_statusbar_new();
        gtk_box_pack_start(GTK_BOX (box), fpsStatusBar, FALSE, TRUE, 0);
        gtk_widget_size_request(fpsStatusBar, &actualSize);

        fpsStatusBar2=gtk_statusbar_new();
        gtk_box_pack_start(GTK_BOX (box), fpsStatusBar2, FALSE, TRUE, 0);
        gtk_widget_size_request(fpsStatusBar2, &actualSize);

        statusbar = gtk_statusbar_new ();
        gtk_box_pack_start (GTK_BOX (box), statusbar, FALSE, TRUE, 0);
        gtk_widget_size_request(statusbar, &actualSize);
        gchar *msg;
        msg=g_strdup_printf("%s", ptr_inputPort->getName().c_str());
        updateStatusbar(statusbar, msg);
        g_free(msg);
    }

    // TimeOut used to refresh the screen
    //timeout_ID = gtk_timeout_add (_options.refreshTime, timeout_CB, NULL);
    //timeout_ID=0;

    timeout_update_ID=gtk_timeout_add(1000, timeout_update_CB, NULL);
    //timeout_ID=0;

    return window;
}

void setOptions(yarp::os::Searchable& options) {
    // switch to subsections if available
    yarp::os::Value *val;
    if (options.check("PortName",val)||options.check("name",val)) {
        ACE_OS::sprintf(_options.portName, "%s", val->asString().c_str());
        fprintf(stderr, "testing name: %s\n", val->asString().c_str());
    }
    if (options.check("NetName",val)||options.check("n",val)) {
        ACE_OS::sprintf(_options.networkName, "%s", val->asString().c_str());
    }
    if (options.check("OutPortName",val)||options.check("out",val)) {
        ACE_OS::sprintf(_options.outPortName, "%s", val->asString().c_str());
    }
    if (options.check("OutNetName",val)||options.check("neto",val)) {
        ACE_OS::sprintf(_options.outNetworkName, "%s", val->asString().c_str());
    }
    if (options.check("RefreshTime",val)||options.check("p",val)) {
        _options.refreshTime = val->asInt();
    }
    if (options.check("PosX",val)||options.check("x",val)) {
        _options.posX = val->asInt();
    }
    if (options.check("PosY",val)||options.check("y",val)) {
        _options.posY = val->asInt();
    }
    if (options.check("Width",val)||options.check("w",val)) {
        _options.windWidth = val->asInt();
    }
    if (options.check("Height",val)||options.check("h",val)) {
        _options.windHeight = val->asInt();
    }
    if (options.check("OutputEnabled",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (options.check("out",val)) {
        _options.outputEnabled = true;
    }
    if (options.check("SaveOptions",val)||options.check("saveoptions",val)) {
        _options.outputEnabled = val->asInt();
    }
    if (options.check("synch"))
    {
        _options.synch=true;
    }
    if (options.check("compact"))
    {
        _options.compact=true;
    }
    if (options.check("keep-above"))
    {
        _options.keepabove=true;
    }
}

void saveOptFile(char *fileName)
{
    FILE *optFile = NULL;
    optFile = ACE_OS::fopen(_options.fileName,"wt");
    if (optFile == NULL)
    {
        g_print("ERROR: Impossible to save to option file.\n");
        return;
    }
    ACE_OS::fprintf(optFile,"PortName %s\n", _options.portName);
    ACE_OS::fprintf(optFile,"NetName %s\n", _options.networkName);
    ACE_OS::fprintf(optFile,"OutPortName %s\n", _options.outPortName);
    ACE_OS::fprintf(optFile,"OutNetName %s\n", _options.outNetworkName);
    ACE_OS::fprintf(optFile,"RefreshTime %d\n", _options.refreshTime);
    ACE_OS::fprintf(optFile,"PosX %d\n", _options.posX);
    ACE_OS::fprintf(optFile,"PosY %d\n", _options.posY);
    ACE_OS::fprintf(optFile,"Width %d\n", _options.windWidth);
    ACE_OS::fprintf(optFile,"Height %d\n", _options.windHeight);
    ACE_OS::fprintf(optFile,"OutputEnables %d\n", _options.outputEnabled);
    ACE_OS::fprintf(optFile,"SaveOptions %d\n", _options.saveOnExit);
    ACE_OS::fprintf(optFile,"synch %d\n", _options.synch);
    ACE_OS::fprintf(optFile,"compact %d\n", _options.compact);
    ACE_OS::fprintf(optFile,"keep-above %d\n", _options.keepabove);
    ACE_OS::fclose(optFile);
}

bool parseParameters(int argc, char* argv[])
{
    yarp::os::Property options;
    if (argc==2)
    {
        if (std::string(argv[1])==std::string("--help"))
        {
            printHelp();
            return false;
        }
    }
    if (argc==2) {
        // user did not use flags, just gave a port name
        // might as well allow this
        options.put("name",argv[1]);
    } else {
        options.fromCommand(argc,argv);
    }
    setOptions(options);
    return true;
}

void setOptionsToDefault()
{
    // Options defaults
    _options.refreshTime = 100;
    ACE_OS::sprintf(_options.portName, "%s","/yarpview/img:i");
    ACE_OS::sprintf(_options.networkName, "%s", "default");
    ACE_OS::sprintf(_options.outPortName, "%s","/yarpview/o:point");
    ACE_OS::sprintf(_options.outNetworkName, "%s", "default");
    _options.outputEnabled = 0;
    _options.windWidth = 300;
    _options.windHeight = 300;
    _options.posX = 100;
    _options.posY = 100;
    ACE_OS::sprintf(_options.fileName, "%s","yarpview.conf");
    _options.saveOnExit = 0;
    _options.compact = 0;
}

bool openPorts()
{
    bool ret = false;

    ptr_inputPort->setReadOnly();
    ret= ptr_inputPort->open(_options.portName);

    if (!ret)
    {
        g_print("Error: port failed to open, quitting.\n");
        return false;
    }

    if (_options.outputEnabled == 1)
    {
        _pOutPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
        g_print("Registering port %s on network %s...\n", _options.outPortName, _options.outNetworkName);
        bool ok = _pOutPort->open(_options.outPortName);
        if (ok) {
            g_print("Port registration succeed!\n");
        }
        else
        {
            g_print("ERROR: Port registration failed.\nQuitting, sorry.\n");
            return false;
        }
    }

    return true;
}

void closePorts()
{

    ptr_inputPort->close();

    if (_options.outputEnabled == 1)
    {
        _pOutPort->close();
        bool ok = true;
        if  (ok)
            g_print("Port %s unregistration succeed!\n", _options.outPortName);
        else
            g_print("ERROR: Port %s unregistration failed.\n", _options.outPortName);
        delete _pOutPort;
        _pOutPort = NULL;
    }
}

void cleanExit()
{
    if (timeout_ID!=0)
        g_source_remove (timeout_ID);
    timeout_ID = 0;

    g_source_remove(timeout_update_ID);
    timeout_update_ID=0;

    gtk_main_quit();
}

//-------------------------------------------------
// Main
//-------------------------------------------------
#undef main //ace leaves a "main" macro defined

int myMain(int argc, char* argv[])
{
    yarp::os::Network yarp;

    //initialize threads in gtk, copied almost verbatim from
    // http://developer.gnome.org/gdk/unstable/gdk-Threads.html

#if !GLIB_CHECK_VERSION(2, 32, 0)
    // since Glib 2.32 g_thread_init is deprecated
    g_thread_init (NULL);
#endif

    gdk_threads_init ();
    gdk_threads_enter ();


    createObjects();

    _frameN = 0;
    _savingSet = false;
    timeout_ID = 0;
    setOptionsToDefault();
    // Parse command line parameters, do this before
    // calling gtk_init(argc, argv) otherwise weird things
    // happens
    if (!parseParameters(argc, argv))
        goto exitRoutine;

    if (!openPorts())
        goto exitRoutine;

    // This is called in all GTK applications. Arguments are parsed
    // from the command line and are returned to the application.
    gtk_init (&argc, &argv);

    // create a new window
    mainWindow = createMainWindow();

    // Non Modal Dialogs
#if GTK_CHECK_VERSION(2,6,0)
    saveSingleDialog = createSaveSingleDialog();
    saveSetDialog = createSaveSetDialog();
#else
    printf("Functionality omitted for older GTK version\n");
#endif
    // Shows all widgets in main Window
    gtk_widget_show_all (mainWindow);
    gtk_window_move(GTK_WINDOW(mainWindow), _options.posX, _options.posY);
    // All GTK applications must have a gtk_main(). Control ends here
    // and waits for an event to occur (like a key press or
    // mouse event).

    ptr_portCallback->attach(&_resources);
    ptr_portCallback->attach(&portFpsData);
    ptr_inputPort->useCallback(*ptr_portCallback);

    if (_options.synch)
    {
        setSynchroMode();
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(synchroDisplayItem), true);
    }
    else
    {
        setTimedMode(_options.refreshTime);
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(synchroDisplayItem), false);
    }

    gtk_main ();

exitRoutine:
    // leave critical section here. From example
    // http://library.gnome.org/devel/gdk/unstable/gdk-Threads.htm
    gdk_threads_leave ();

    closePorts();
    if (_options.saveOnExit != 0)
        saveOptFile(_options.fileName);

    deleteObjects();

    return 0;
}

#ifdef YARP_WIN32_NOCONSOLE
#include <windows.h>
// win32 non-console applications define WinMain as the
// entry point for the linker
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    return myMain (__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
    return myMain(argc, argv);
}
#endif


void printHelp()
{
    g_print("yarpview usage:\n");
    g_print("  --name: input port name (default: /yarpview/img:i)\n");
    g_print("  --x: x position of the window in the screen\n");
    g_print("  --y: y position of the window in the screen\n");
    g_print("  --w: width of the window\n");
    g_print("  --h: height of the window\n");
    g_print("  --p: refresh time [ms]\n");
    g_print("  --synch: synchronous display, every image received by the input port is displayed\n");
    g_print("  --out: output port name (if this option is not specified the port is not created)\n");
    g_print("  --neto: output network\n");
    g_print("  --neti: input network\n");
    g_print("  --compact: if this flag is enabled, no window controls will be printed\n");
    g_print("  --keep-above: keep windows above others\n");
}

#include <yarp/sig/ImageFile.h>
void saveCurrentFrame()
{
    static unsigned int count=0;
    std::string tmp;
    tmp="frame";
    std::stringstream lStream;
    lStream.fill('0');
    lStream.width(3);
    lStream << count;
    tmp+=lStream.str();
    tmp+=".ppm";
    yarp::sig::ImageOf<yarp::sig::PixelRgb> frame;

    if (_resources.getLastImage(frame))
    {
        yarp::sig::file::write(frame, tmp.c_str());
        count++;
    }
}
