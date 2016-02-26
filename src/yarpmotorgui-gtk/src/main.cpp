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


///////////YARP//////////
//#include <yarp/os/impl/NameClient.h>

///////canGui/////////////
//#include "include/yarpmotorgui.h"
//#include "include/partMover.h"

#include <yarp/dev/Drivers.h>
#include "include/allPartsWindow.h"
#include <string>
#include <string.h>


///////Initializations////
GtkWidget *robotNameBox   = NULL;

char *partsName[];
int *ENA[];
int NUMBER_OF_ACTIVATED_PARTS = 0;
int NUMBER_OF_ACTIVATED_CARTESIAN = 0;
int NUMBER_OF_AVAILABLE_PARTS = 0;
int PART;
bool debug_param_enabled = false;
bool speedview_param_enabled =false;
bool enable_calib_all =false;
bool position_direct_enabled = false;
bool openloop_enabled = false;
bool old_impedance_enabled = false;

ResourceFinder *finder;
////////////////////////

static void destroy_main (GtkWindow *window,    gpointer   user_data)
{
    gtk_widget_destroy (GTK_WIDGET(window));
    gtk_main_quit ();
}

GtkWidget *buttonGoAll;
GtkWidget *buttonSeqAll;
GtkWidget *buttonSeqAllTime;
GtkWidget *buttonSeqAllSave;
GtkWidget *buttonSeqAllLoad;
GtkWidget *buttonSeqAllCycle;
GtkWidget *buttonSeqAllCycleTime;
GtkWidget *buttonSeqAllStop;
GtkWidget *buttonSeqAllStopTime;
GtkWidget *buttonRunAllParts;
GtkWidget *buttonHomeAllParts;

GtkWidget *buttonCrtSeqAllSave;
GtkWidget *buttonCrtSeqAllLoad;
GtkWidget *buttonCrtSeqAllCycleTime;
GtkWidget *buttonCrtSeqAllStop;

//*********************************************************************************
// This callback switches among tabs
void notebook_change (GtkNotebook *nb, GtkNotebookPage *nbp,    gint current_enabled, partMover** currentPartMover)
{
    gint i;

    //skip if the "all" tab has been called
    if (current_enabled<NUMBER_OF_ACTIVATED_PARTS)
        {
            for (i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
                {
                    //fprintf(stderr, "disabling update for part %d \n", i);
                    currentPartMover[i]->disable_entry_update(currentPartMover[i]);
                }
            //fprintf(stderr, "enabling update for part %d \n", current_enabled);
            currentPartMover[current_enabled]->enable_entry_update(currentPartMover[current_enabled]);
            currentPartMover[current_enabled]->first_time=true;
        }

    return;
}

//*********************************************************************************
#ifdef CARTESIAN_MOVER
void add_enabled_joints(cartesianMover* cm, GtkWidget *vbox)
{
    GtkWidget *check= gtk_check_button_new_with_mnemonic ("test");
    gtk_fixed_put   (GTK_FIXED(vbox), check, 10, 0);
    gtk_widget_set_size_request     (check, 80, 50);
    gtk_toggle_button_set_active((GtkToggleButton*) check, true);
    //g_signal_connect (check, "clicked", G_CALLBACK (check_pressed),ENA[n]);
}
#endif

//*********************************************************************************
// This function is main window after selection of the part to be controlled

static void myMain2(GtkButton *button,  int *position)
{
    std::string robotName;
    std::string portLocalName;
    std::string portLocalName2;

    GtkWidget *main_vbox1        = NULL;
    GtkWidget *main_vbox2        = NULL;
    GtkWidget *main_vbox3        = NULL;
    GtkWidget *main_vbox4        = NULL;
    GtkWidget *main_vbox5        = NULL;
    Property options;

    yarp::os::Network::init();

    //retrieve robot name
    robotName = gtk_entry_get_text((GtkEntry *)(robotNameBox));
    //sprintf(&robotName[0], "%s", gtk_entry_get_text((GtkEntry *)(robotNameBox)));

    gtk_widget_destroy (window);
    window = NULL;
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    //Creation of the notebook
    GtkWidget*  nb1 = gtk_notebook_new();
    gtk_container_add (GTK_CONTAINER (window), nb1);
    g_signal_connect (window, "destroy",G_CALLBACK (destroy_main), &window);
    gtk_window_set_title (GTK_WINDOW (window), "Robot Motor GUI V1.10");

    char legsLabel[]= "Legs";
    GtkWidget *label;
    int n;

    PolyDriver *partsdd[MAX_NUMBER_ACTIVATED];
    PolyDriver *debugdd[MAX_NUMBER_ACTIVATED];
    partMover *partMoverList[MAX_NUMBER_ACTIVATED];
    partMover *currentPartMover;

    #ifdef CARTESIAN_MOVER
    PolyDriver *cartesiandd[MAX_NUMBER_ACTIVATED];
    cartesianMover *cartesianMoverList[MAX_NUMBER_ACTIVATED];
    cartesianMover *currentCartesianMover;
    #endif

    for (n = 0; n < NUMBER_OF_AVAILABLE_PARTS; n++)
        {
            if(*ENA[n] == 1)
                {
                    label = gtk_label_new(partsName[n]);
                    main_vbox4 = gtk_vbox_new (FALSE, 0);
                    gint note4 = gtk_notebook_append_page((GtkNotebook*) nb1, main_vbox4, label);

                    std::string robotPartPort= "/";
                    robotPartPort += robotName.c_str();
                    robotPartPort += "/";
                    robotPartPort += partsName[n];

                    std::string robotPartDebugPort= "/";
                    robotPartDebugPort += robotName.c_str();
                    robotPartDebugPort += "/debug/";
                    robotPartDebugPort += partsName[n];

                    //checking existence of the port
                    int ind = 0;
                    portLocalName="/";
                    portLocalName+=robotName.c_str();
                    portLocalName+="/yarpmotorgui";
                    char tmp[80];
                    sprintf(tmp, "%d", ind);
                    portLocalName+=tmp;
                    portLocalName+="/";
                    portLocalName+=partsName[n];
                    // sprintf(&portLocalName[0], "/%s/gui%d/%s", robotName.c_str(), ind, partsName[n]);

                    std::string nameToCheck = portLocalName;
                    nameToCheck += "/rpc:o";

                    //   NameClient &nic=NameClient::getNameClient();
                    fprintf(stderr, "Checking the existence of: %s \n", nameToCheck.c_str());
                    //                    Address adr=nic.queryName(nameToCheck.c_str());

                    Contact adr=Network::queryName(nameToCheck.c_str());

                    //Contact c = yarp::os::Network::queryName(portLocalName.c_str());
                    fprintf(stderr, "ADDRESS is: %s \n", adr.toString().c_str());
                    while(adr.isValid())
                        {
                            ind++;

                            portLocalName="/";
                            portLocalName+=robotName.c_str();
                            portLocalName+="/yarpmotorgui";
                            char tmp[80];
                            sprintf(tmp, "%d", ind);
                            portLocalName+=tmp;
                            portLocalName+="/";
                            portLocalName+=partsName[n];
                            // sprintf(&portLocalName[0], "/%s/gui%d/%s", robotName.c_str(), ind, partsName[n]);
                            nameToCheck = portLocalName;
                            nameToCheck += "/rpc:o";
                            // adr=nic.queryName(nameToCheck.c_str());
                            adr=Network::queryName(nameToCheck.c_str());
                        }

                    options.put("local", portLocalName.c_str());
                    options.put("device", "remote_controlboard");
                    options.put("remote", robotPartPort.c_str());
                    options.put("carrier", "udp");
                    partsdd[n] = new PolyDriver(options);

                    if (debug_param_enabled)
                    {
                        Property debugOptions;
                        portLocalName2=portLocalName;
                        // the following complex line of code performs a substring substitution (see example below)
                        // "/icub/yarpmotorgui2/right_arm" -> "/icub/yarpmotorgui2/debug/right_arm"
                        portLocalName2.replace(portLocalName2.find(partsName[n]),strlen(partsName[n]),std::string("debug/")+std::string(partsName[n]));
                        debugOptions.put("local", portLocalName2.c_str());
                        debugOptions.put("device", "debugInterfaceClient");
                        debugOptions.put("remote", robotPartPort.c_str());
                        debugOptions.put("carrier", "udp");
                        debugdd[n] = new PolyDriver(debugOptions);
                        if(debugdd[n]->isValid() == false)
                          {
                              fprintf(stderr, "Problems opening the debug client \n");
                          }
                    }
                    else
                    {
                        debugdd[n]=0;
                    }

                    currentPartMover = new partMover(main_vbox4, partsdd[n], debugdd[n], partsName[n], finder,speedview_param_enabled, enable_calib_all);
                    if(!(currentPartMover->interfaceError))
                        {
                            partMoverList[NUMBER_OF_ACTIVATED_PARTS] = currentPartMover;
                            NUMBER_OF_ACTIVATED_PARTS++;
                        }
                    else
                        fprintf(stderr, "Trying to exit without starting the GUI \n");
                }
        }

    if (NUMBER_OF_ACTIVATED_PARTS > 0)
        {

            #ifdef CARTESIAN_MOVER
            for (n = 0; n < NUMBER_OF_AVAILABLE_PARTS; n++)
                {
                    if(*ENA[n] == 1)
                        {
                            if (finder->check("cartesian"))
                                {
                                    Bottle bCartesianParts = finder->findGroup("cartesian");
                                    if (bCartesianParts.check(partsName[n]))
                                        {
                                            fprintf(stderr, "Adding cartesian tab %d\n", n);
                                            std::string cartesianPartName;
                                            cartesianPartName = partsName[n];
                                            cartesianPartName += "_cartesian";
                                            label = gtk_label_new(cartesianPartName.c_str());
                                            main_vbox4 = gtk_vbox_new (FALSE, 0);
                                            gint note4 = gtk_notebook_append_page((GtkNotebook*) nb1, main_vbox4, label);

                                            std::string robotPartPort= "/";
                                            robotPartPort = robotPartPort + robotName.c_str() + "/cartesianController/" + partsName[n];

                                            //checking eixstence of the port
                                            int ind = 0;
                                            sprintf(&portLocalName[0], "/%s/yarpmotorgui%d/cartesian/%s", robotName.c_str(), ind, partsName[n]);
                                            // NameClient &nic=NameClient::getNameClient();
                                            std::string nameToCheck = portLocalName.c_str();
                                            nameToCheck += "/rpc:o";
                                            fprintf(stderr, "Checking the existence of: %s \n", nameToCheck.c_str());
                                            //                                           Address adr=nic.queryName(nameToCheck.c_str());

                                            Contact adr = yarp::os::Network::queryName(nameToCheck.c_str());
                                            fprintf(stderr, "ADDRESS is: %s \n", adr.toString().c_str());
                                            while(adr.isValid())
                                                {
                                                    ind++;
                                                    sprintf(&portLocalName[0], "/%s/yarpmotorgui%d/cartesian/%s", robotName.c_str(), ind, partsName[n]);
                                                    nameToCheck=portLocalName.c_str();
                                                    nameToCheck += "/rpc:o";
                                                    //Contact adr=yarp::os::Network::queryName(portLocalName.c_str());
                                                    fprintf(stderr, "Checking the existence of: %s \n", nameToCheck.c_str());
                                                    adr=yarp::os::Network::queryName(nameToCheck.c_str());
                                                    //adr=nic.queryName(nameToCheck.c_str());
                                                }

                                            options.put("local", portLocalName.c_str());    //local port names
                                            options.put("device", "cartesiancontrollerclient");
                                            options.put("remote", robotPartPort.c_str());

                                            fprintf(stderr, "Trying to open the cartesian PolyDriver...\n");
                                            cartesiandd[n] = new PolyDriver(options);
                                            fprintf(stderr, "Checking the validity of the cartesian PolyDriver...\n");
                                            if(cartesiandd[n]->isValid())
                                                {
                                                    currentCartesianMover = new cartesianMover(main_vbox4, cartesiandd[n], partsName[n], finder);
                                                    if(!(currentCartesianMover->interfaceError))
                                                        {
                                                            cartesianMoverList[NUMBER_OF_ACTIVATED_CARTESIAN] = currentCartesianMover;
                                                            NUMBER_OF_ACTIVATED_CARTESIAN++;
                                                        }
                                                    else
                                                        fprintf(stderr, "One of the requested cartesian interfaces was not available\n");
                                                }
                                            else
                                                fprintf(stderr, "Cartesian Poly Driver was not valid \n");
                                        }
                                    else
                                        fprintf(stderr, "A cartesian interface was requested but no part was associated \n");
                                }
                            else
                                fprintf(stderr, "GUI was not configured for cartesian \n");
                        }
                }
            #endif

            if (NUMBER_OF_ACTIVATED_PARTS>0)
                {
                    fprintf(stderr, "Activating tabs \n");
                    g_signal_connect (nb1, "switch-page",G_CALLBACK(notebook_change), partMoverList);

                    main_vbox5 = gtk_fixed_new ();
                    fprintf(stderr, "Created all tab box\n");
                    label = gtk_label_new("all");
                    fprintf(stderr, "Appending all tab \n");
                    gint note5 = gtk_notebook_append_page((GtkNotebook*) nb1, main_vbox5, label);

                    //Frame
                    fprintf(stderr, "Appending all frame \n");
                    GtkWidget *frame1 = gtk_frame_new ("Global joint commands");
                    gtk_widget_set_size_request     (frame1, 250, 550);
                    gtk_fixed_put (GTK_FIXED (main_vbox5), frame1, 10, 10);

                    //Button 1 in the panel
                    buttonGoAll = gtk_button_new_with_mnemonic ("Go ALL!");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonGoAll, 30, 50);
                    g_signal_connect (buttonGoAll, "clicked", G_CALLBACK (go_all_click), partMoverList);
                    gtk_widget_set_size_request(buttonGoAll, 190, 30);

                    //Button 2 in the panel
                    buttonSeqAll = gtk_button_new_with_mnemonic ("Run ALL Sequence");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAll, 30, 100);
                    g_signal_connect (buttonSeqAll, "clicked",G_CALLBACK(sequence_all_click), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAll, 190, 30);

                    //Button 2time in the panel
                    buttonSeqAllTime = gtk_button_new_with_mnemonic ("Run ALL Sequence (time)");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllTime, 30, 150);
                    g_signal_connect (buttonSeqAllTime, "clicked",G_CALLBACK(sequence_all_click_time), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllTime, 190, 30);

                    //Button 3 in the panel
                    buttonSeqAllSave = gtk_button_new_with_mnemonic ("Save ALL Sequence");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllSave, 30, 200);
                    g_signal_connect (buttonSeqAllSave, "clicked", G_CALLBACK(sequence_all_save), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllSave, 190, 30);

                    //Button 4 in the panel
                    buttonSeqAllLoad = gtk_button_new_with_mnemonic ("Load ALL Sequence");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllLoad, 30, 250);
                    g_signal_connect (buttonSeqAllLoad, "clicked", G_CALLBACK (sequence_all_load), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllLoad, 190, 30);

                    //Button 5 in the panel
                    buttonSeqAllCycle = gtk_button_new_with_mnemonic ("Cycle ALL Sequence");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllCycle, 30, 300);
                    g_signal_connect (buttonSeqAllCycle, "clicked", G_CALLBACK (sequence_all_cycle), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllCycle, 190, 30);

                    //Button 5time in the panel
                    buttonSeqAllCycleTime = gtk_button_new_with_mnemonic ("Cycle ALL Sequence (time)");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllCycleTime, 30, 350);
                    g_signal_connect (buttonSeqAllCycleTime, "clicked", G_CALLBACK (sequence_all_cycle_time), partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllCycleTime, 190, 30);

                    //Button 6 in the panel
                    buttonSeqAllStop = gtk_button_new_with_mnemonic ("Stop ALL Sequence");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonSeqAllStop, 30, 400);
                    g_signal_connect (buttonSeqAllStop, "clicked", G_CALLBACK (sequence_all_stop),  partMoverList);
                    gtk_widget_set_size_request     (buttonSeqAllStop, 190, 30);

                    //Button 6time in the panel
                    //buttonSeqAllStopTime = gtk_button_new_with_mnemonic ("Stop ALL Sequence (time)");
                    //gtk_fixed_put (GTK_FIXED(main_vbox5), buttonSeqAllStopTime, 120, 450);
                    //g_signal_connect (buttonSeqAllStopTime, "clicked", G_CALLBACK (sequence_all_stop_time),  partMoverList);
                    //gtk_widget_set_size_request     (buttonSeqAllStopTime, 190, 30);

                    //Button 6time in the panel
                    buttonRunAllParts = gtk_button_new_with_mnemonic ("Run ALL Parts");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonRunAllParts, 30, 450);
                    g_signal_connect (buttonRunAllParts, "clicked", G_CALLBACK (run_all_parts),  partMoverList);
                    gtk_widget_set_size_request     (buttonRunAllParts, 190, 30);

                    //Button 7 in the panel
                    buttonHomeAllParts = gtk_button_new_with_mnemonic ("Home ALL Parts");
                    gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonHomeAllParts, 30, 500);
                    g_signal_connect (buttonHomeAllParts, "clicked", G_CALLBACK (home_all_parts),  partMoverList);
                    gtk_widget_set_size_request     (buttonHomeAllParts, 190, 30);

                    //Global cartesian commands
                    if(NUMBER_OF_ACTIVATED_CARTESIAN>0)
                        {
                            //Frame
                            fprintf(stderr, "Appending all cartesian frame \n");
                            GtkWidget *frame2 = gtk_frame_new ("Global cartesian commands");
                            gtk_widget_set_size_request     (frame2, 250, 550);
                            gtk_fixed_put (GTK_FIXED (main_vbox5), frame2, 300, 10);

                            //for(int i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
                            //    add_enabled_joints(cartesianMoverList[i], main_vbox5);

                            //Button 3 in the panel
                            #ifdef CARTESIAN_MOVER
                            buttonCrtSeqAllSave = gtk_button_new_with_mnemonic ("Save ALL Cartesian Seq");
                            gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonCrtSeqAllSave, 320, 200);
                            g_signal_connect (buttonCrtSeqAllSave, "clicked", G_CALLBACK(sequence_crt_all_save), cartesianMoverList);
                            gtk_widget_set_size_request     (buttonCrtSeqAllSave, 190, 30);

                            //Button 4 in the panel
                            buttonCrtSeqAllLoad = gtk_button_new_with_mnemonic ("Load ALL Cartesian Seq");
                            gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonCrtSeqAllLoad, 320, 250);
                            g_signal_connect (buttonCrtSeqAllLoad, "clicked", G_CALLBACK (sequence_crt_all_load), cartesianMoverList);
                            gtk_widget_set_size_request     (buttonCrtSeqAllLoad, 190, 30);

                            //Button 5time in the panel
                            buttonCrtSeqAllCycleTime = gtk_button_new_with_mnemonic ("Cycle ALL Cartesian Seq");
                            gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonCrtSeqAllCycleTime, 320, 350);
                            g_signal_connect (buttonCrtSeqAllCycleTime, "clicked", G_CALLBACK (sequence_crt_all_cycle_time), cartesianMoverList);
                            gtk_widget_set_size_request     (buttonCrtSeqAllCycleTime, 190, 30);

                            //Button 6 in the panel
                            buttonCrtSeqAllStop = gtk_button_new_with_mnemonic ("Stop ALL Cartsian Seq");
                            gtk_fixed_put   (GTK_FIXED(main_vbox5), buttonCrtSeqAllStop, 320, 400);
                            g_signal_connect (buttonCrtSeqAllStop, "clicked", G_CALLBACK (sequence_crt_all_stop),  cartesianMoverList);
                            gtk_widget_set_size_request     (buttonCrtSeqAllStop, 190, 30);
                            #endif
                        }
                    // finish & show
                    //  connected_status();
                    fprintf(stderr, "Resizing window \n");
                    gtk_window_set_default_size (GTK_WINDOW (window), 480, 250);
                    gtk_window_set_resizable (GTK_WINDOW (window), true);
                }
        }

    fprintf(stderr, "Making the window visible \n");
    if (!GTK_WIDGET_VISIBLE (window))
        gtk_widget_show_all (window);
    else
        {
            gtk_widget_destroy (window);
            window = NULL;
        }

    gtk_main ();
    fprintf(stderr, "Closing the partMovers. Number of activated parts was %d. \n", NUMBER_OF_ACTIVATED_PARTS);
    for (int i = 0; i < NUMBER_OF_ACTIVATED_PARTS; i++)
        {
            fprintf(stderr, "Closing part number %d \n", i);
            partMoverList[i]->releaseDriver();
            fprintf(stderr, "Deleting part number %d \n", i);
            delete partMoverList[i];
            Time::delay(0.1);
        }

    #ifdef CARTESIAN_MOVER
    fprintf(stderr, "Closing the cartesianMovers. Number of activated parts was %d. \n", NUMBER_OF_ACTIVATED_CARTESIAN);
    for (int i = 0; i < NUMBER_OF_ACTIVATED_CARTESIAN; i++)
        {
            fprintf(stderr, "Closing part number %d \n", i);
            cartesianMoverList[i]->releaseDriver();
            fprintf(stderr, "Deleting part number %d \n", i);
            delete cartesianMoverList[i];
            Time::delay(0.1);
        }
    #endif

    fprintf(stderr, "Closing the main GUI \n");
    Network::fini();
    return;

}


//*********************************************************************************
static GtkTreeModel * create_net_model (void)
{
    gint i = 0;
    GtkListStore *store;
    GtkTreeIter iter;

    // create list store
    store = gtk_list_store_new (1, G_TYPE_STRING);

    // add data to the list store
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, "Right Arm",-1);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, "Left Arm",-1);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, "Head Waist",-1);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, "Legs",-1);

    return GTK_TREE_MODEL (store);
}

//*********************************************************************************
static void combo_net_changed (GtkComboBox *box,    gpointer   user_data)
{
    PART = gtk_combo_box_get_active (box);
}

static void check_pressed(GtkWidget *box,   gpointer   user_data)
{
    int *pENA = (int *) user_data;
    if (*pENA == 0)
        *pENA = 1;         //part selected for use
    else
        *pENA = 0;         //part deselected for use
    //fprintf(stderr, "%d \n", *pENA);
    return;
}


//*********************************************************************************
// Entry point for the GTK application
int myMain( int   argc, char *argv[] )
{
    int n=0;
    GtkWidget *button1;
    GtkWidget *inv1     = NULL;
    GtkWidget *top_hbox = NULL;
    GtkWidget *main_vbox= NULL;
    Property p, q;
    finder = new ResourceFinder;
    gtk_init (&argc, &argv);
    //////////////////////////////////////////////////////////////////////
    //create the main window, and sets the callback destroy_main() to quit
    //the application when the main window is closed
    //////////////////////////////////////////////////////////////////////
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "Robot Motor GUI V1.10");
    g_signal_connect (window, "destroy",G_CALLBACK (destroy_main), &window);

    gtk_container_set_border_width (GTK_CONTAINER (window), 8);

    //Creation of main_vbox the container for every other widget
    main_vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), main_vbox);

    //creation of the top_hbox
    top_hbox = gtk_hbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (top_hbox), 10);
    gtk_container_add (GTK_CONTAINER (main_vbox), top_hbox);

    inv1 = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (top_hbox), inv1);

    //retrieve information for the list of parts
    finder->setVerbose();
    finder->setDefaultConfigFile("yarpmotorgui.ini");
    finder->setDefault("name", "icub");
    finder->configure(argc,argv);
    //fprintf(stderr, "Retrieved finder: %p \n", finder);
    if (finder->check("calib"))
    {
        printf("Calibrate buttons on\n");
        enable_calib_all = true;
    }
    if (finder->check("admin"))
    {
        printf("Admin mode on.\n");
        enable_calib_all = true;
        debug_param_enabled = true;
        position_direct_enabled = true;
        openloop_enabled = true;
        old_impedance_enabled = true;
    }
    if (finder->check("debug"))
    {
        printf("Debug interface requested.\n");
        debug_param_enabled = true;
    }
    if (finder->check("speed"))
    {
        printf("Speed view requested.\n");
        speedview_param_enabled = true;
    }
    if (finder->check("direct"))
    {
        printf("Position direct requested.\n");
        position_direct_enabled = true;
    }
    if (finder->check("openloop"))
    {
        printf("Openloop requested.\n");
        openloop_enabled = true;
    }

    bool deleteParts=false;
    std::string robotName=finder->find("name").asString().c_str();
    Bottle *pParts=finder->find("parts").asList();
    if (pParts==NULL)
    {
        printf("Setting default parts.\n");
        pParts=new Bottle("head torso left_arm right_arm left_leg right_leg");
        deleteParts=true;
    }

    NUMBER_OF_AVAILABLE_PARTS=pParts->size();
    if (NUMBER_OF_AVAILABLE_PARTS > MAX_NUMBER_ACTIVATED)
        {
            fprintf(stderr, "The number of parts exceeds the maximum! \n");
            return 0;
        }
    if (NUMBER_OF_AVAILABLE_PARTS<=0)
        {
            fprintf(stderr, "Invalid number of parts, check config file \n");
            return 0;
        }

    for(n=0; n < MAX_NUMBER_ACTIVATED; n++)
        {
            //ENA = 0: part available
            //ENA = -1: part unavailable
            //ENA = 1: part used
            ENA[n] = new int;
            *ENA[n] = -1;
        }

    //Check 1 in the panel
    for(n=0;n<NUMBER_OF_AVAILABLE_PARTS;n++)
        {
            partsName[n] = new char[80];
            *ENA[n] = 1;
            //fprintf(stderr, "Getting part %d \n", n);
            strcpy(partsName[n], pParts->get(n).asString().c_str());
            //fprintf(stderr, "%s \n", partsName[n]);
            GtkWidget *check= gtk_check_button_new_with_mnemonic (partsName[n]);
            gtk_fixed_put   (GTK_FIXED(inv1), check, 100*n, 0);
            gtk_widget_set_size_request     (check, 80, 50);
            gtk_toggle_button_set_active((GtkToggleButton*) check, true);
            g_signal_connect (check, "clicked", G_CALLBACK (check_pressed),ENA[n]);
        }

    //Robot name
    robotNameBox = gtk_entry_new ();
    gtk_entry_set_editable(GTK_ENTRY(robotNameBox),true);
    //fprintf(stderr, "%s", (const char*) xtmp);
    gtk_entry_set_text(GTK_ENTRY(robotNameBox), robotName.c_str());
    gtk_fixed_put   (GTK_FIXED(inv1), robotNameBox, 100*NUMBER_OF_AVAILABLE_PARTS, 10);
    //g_signal_connect (renderer, "edited", G_CALLBACK (edited_timing), currentClassData);
    gtk_widget_set_size_request     (robotNameBox, 60, 30);
    //gtk_container_add (GTK_CONTAINER (top_hbox), robotNameBox);

    //Button 1 in the panel
    button1 = gtk_button_new_with_mnemonic ("Select Parts and Click");
    //gtk_container_add (GTK_CONTAINER (top_hbox ), button1);
    gtk_fixed_put   (GTK_FIXED(inv1), button1, 100*(NUMBER_OF_AVAILABLE_PARTS +1), 10);
    g_signal_connect (button1, "clicked", G_CALLBACK (myMain2),NULL);
    gtk_widget_set_size_request     (button1, 180, 30);

    gtk_window_set_default_size (GTK_WINDOW (window), 60, 30);
    gtk_window_set_resizable (GTK_WINDOW (window), true);

    if (!GTK_WIDGET_VISIBLE (window))
        gtk_widget_show_all (window);
    else
        {
            gtk_widget_destroy (window);
            window = NULL;
        }

    gtk_main ();
    fprintf(stderr, "Deleting the finder");
    delete finder;
    fprintf(stderr, "...done!\n");

    if (deleteParts)
        delete pParts;

    return 0;
}

int main(int argc, char* argv[])
{
    return myMain(argc, argv);
}
