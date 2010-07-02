#include "include/robotMotorGui.h"
#include "include/cartesianMover.h"
#include <string.h>

static void toggle_tracking_mode(GtkWidget *box,	gpointer   user_data)
{
    ICartesianControl *crt = (ICartesianControl *) user_data;
    bool trck;
    crt->getTrackingMode(&trck);
    if (trck)
        crt->setTrackingMode(false);        
    else
        crt->setTrackingMode(true);        
    return;
}

static void stop_motion(GtkWidget *box,	gpointer   user_data)
{
    ICartesianControl *crt = (ICartesianControl *) user_data;
    bool stopped = crt->stopControl();
    if(!stopped)
        fprintf(stderr, "There were problems in trying stopping cartesian control\n");
    return;
}

bool cartesianMover::display_cartesian_pose(cartesianMover *cm)
{

    ICartesianControl *icrt = cm->crt;
    GtkEntry **entry = (GtkEntry **) cm->currPosArray;
    GtkWidget **sliderPosArray = (GtkWidget **) cm->sliderArray;

    //fprintf(stderr, "Trying to get the cartesian position...");
    Vector x;
    Vector axis;
    if(!icrt->getPose(x,axis))
        fprintf(stderr, "Troubles in gettin the cartesian pose for %s", cm->partLabel);
    //else
    //fprintf(stderr, "got x=%s, o=%s\n", x.toString().c_str(), o.toString().c_str());
  
    //converting to a Rotation Matrix
    Matrix R = axis2dcm(axis);
    //converting to a Euler angles
    Vector eulerAngles = dcm2euler(R, 1);
    //back to the Roation Matrix
    //Matrix Rr = euler2dcm(eulerAngles);

    //fprintf(stderr, "v: %s\n Res Matrix: %s\n", eulerAngles.toString().c_str(), (Rr-R).toString().c_str());
     
    gboolean focus = false;
    char buffer[40] = {'i', 'n', 'i', 't'};
    for (int k = 0; k < NUMBER_OF_CARTESIAN_COORDINATES; k++)
        {
            if (k<3)
                sprintf(buffer, "%.2f", x(k));  
            if (k>=3 && k<= 5)
                sprintf(buffer, "%.1f", eulerAngles(k-3)*180/M_PI);    
            gtk_entry_set_text((GtkEntry*) entry[k],  buffer);
      
            //if changing orientation do not update all the three sliders
            //if changing position update other sliders
            if(k<3)
                {
                    g_object_get((gpointer) sliderPosArray[k], "has-focus", &focus, NULL);
                    if(!focus)
                        gtk_range_set_value 			((GtkRange *) (sliderPosArray[k]),  x(k));
                }
            else
                {
                    for (int i = 3; i < NUMBER_OF_CARTESIAN_COORDINATES; i++)
                        {
                            gboolean tmp;
                            g_object_get((gpointer) sliderPosArray[i], "has-focus", &tmp, NULL);
                            focus = focus || tmp;
                        }
                    if(!focus)
                        gtk_range_set_value 			((GtkRange *) (sliderPosArray[k]),  eulerAngles(k-3) * 180/M_PI);
                }
      
        }
    return true;
}

bool cartesianMover::display_axis_pose(cartesianMover *cm)
{

    ICartesianControl *icrt = cm->crt;
    GtkEntry **entry = (GtkEntry **) cm->po;

    //fprintf(stderr, "Trying to get the cartesian position...");
    Vector x;
    Vector axis;
    if(!icrt->getPose(x,axis))
        fprintf(stderr, "Troubles in gettin the cartesian pose for %s", cm->partLabel);
    //else
    //fprintf(stderr, "got x=%s, o=%s\n", x.toString().c_str(), o.toString().c_str());
  
    char buffer[40] = {'i', 'n', 'i', 't'};
    for (int k = 0; k < 4; k++)
        {
            if (k == 3)
                sprintf(buffer, "%.2f", axis(k)*180/M_PI);
            else
                sprintf(buffer, "%.2f", axis(k));    
            gtk_entry_set_text((GtkEntry*) entry[k],  buffer);

        }
    return true;
}

void cartesianMover::position_slider_changed(GtkRange *range, cartesianMover *cm)
{

    ICartesianControl *icrt = cm->crt;
    GtkWidget **sliderPosArray = (GtkWidget **) cm->sliderArray;
    GtkWidget *sliderVel      = (GtkWidget *) cm->sliderVelocity;
  
    int sliderIndex;
    for(int i=0; i < NUMBER_OF_CARTESIAN_COORDINATES; i++)
        if(sliderPosArray[i]==(GtkWidget*) range)
            sliderIndex=i;

    gboolean focus;
    g_object_get((gpointer) range, "has-focus", &focus, NULL);
    if (focus)
        {
            if (sliderIndex<3)
                {
                    Vector xd(3);   Vector x;   Vector o;
                    xd(0) = gtk_range_get_value((GtkRange *) sliderPosArray[0]);
                    xd(1) = gtk_range_get_value((GtkRange *) sliderPosArray[1]);  
                    xd(2) = gtk_range_get_value((GtkRange *) sliderPosArray[2]); 

                    if(!icrt->getPose(x,o))
                        fprintf(stderr, "Troubles in gettin the cartesian pose for %s\n", cm->partLabel);

                    double time = gtk_range_get_value((GtkRange *) sliderVel);
                    if(!icrt->goToPoseSync(xd,o,time))
                        fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
                    //else
                    //    fprintf(stderr, "Sent %s new cartesian for %s with time %.1f\n", xd.toString().c_str(),cm->partLabel, time);

                    int count= 0;
                    bool checkDone=false;
                    while(checkDone==false && count*10<2*time*1000)
                        {
                            icrt->checkMotionDone(&checkDone);
                            //fprintf(stderr, "Check %d\n", checkDone);
                            Time::delay(0.01);
                            count++;
                        }
                    if(!checkDone)
                        {
                            //fprintf(stderr, "Aborting since trajectory was not completed in 2*Time\n");
                            dialog_message(GTK_MESSAGE_ERROR,(char *)"Aborting cartesian trajectory", (char *)"Final point reaching was taking too much time", true);
                            icrt->stopControl();
                        }
                }
                        
            else
                {
                    Vector eud(3);  Vector x;   Vector o;
                    eud(0) = gtk_range_get_value((GtkRange *) sliderPosArray[3]) * M_PI/180;
                    eud(1) = gtk_range_get_value((GtkRange *) sliderPosArray[4]) * M_PI/180;  
                    eud(2) = gtk_range_get_value((GtkRange *) sliderPosArray[5]) * M_PI/180;  

                    //fprintf(stderr, "Will move to x=%s, o=%s\n", x.toString().c_str(), o.toString().c_str());
                    Matrix Rd = euler2dcm(eud);
                    Vector od = dcm2axis(Rd);

                    if(!icrt->getPose(x,o))
                        fprintf(stderr, "Troubles in gettin the cartesian pose for %s\n", cm->partLabel);

                    double time = gtk_range_get_value((GtkRange *) sliderVel);
                    if(!icrt->goToPoseSync(x,od,time))
                        fprintf(stderr, "Troubles in executing the cartesian pose for %s\n", cm->partLabel);
                    //else
                    //    fprintf(stderr, "Sent %s new orientation for %s\n", od.toString().c_str(), cm->partLabel);

                    int count= 0;
                    bool checkDone=false;
                    while(checkDone==false && count*10<2*time*1000)
                        {
                            icrt->checkMotionDone(&checkDone);
                            //fprintf(stderr, "Check %d\n", checkDone);
                            Time::delay(0.01);
                            count++;
                        }
                        
                    if(!checkDone)
                        {
                            //fprintf(stderr, "Aborting since trajectory was not completed in 2*Time\n");
                            dialog_message(GTK_MESSAGE_ERROR,(char *)"Aborting cartesian trajectory", (char *)"Final point reaching was taking too much time", true);
                            icrt->stopControl();
                        }
                }
        }
}

cartesianMover::cartesianMover(GtkWidget *vbox_d, PolyDriver *partDd_d, char *partName, ResourceFinder *fnd)
{

    finder = fnd;

    if (!finder->isNull())
        fprintf(stderr, "Setting a valid finder \n");

    partLabel = partName;
    partDd = partDd_d;
    vbox = vbox_d;
    interfaceError = false;

    if (!partDd->isValid()) {
        fprintf(stderr, "Device given to the cartesian interface is not available.\n");
        interfaceError = true;
    }

    fprintf(stderr, "Opening crt interface...");
    bool ok;
    ok  = partDd->view(crt);
    if ((!ok) || (crt==0))
        fprintf(stderr, "...crt was not ok...ok=%d", ok);

    if (!partDd->isValid()) {
        fprintf(stderr, "Cartesian device driver was not valid! \n");
        dialog_severe_error(GTK_MESSAGE_ERROR,(char *) "Cartesian device not available.", (char *) "Check available devices", true);
        interfaceError = true;
    }
    else if (!ok) {
        fprintf(stderr, "Error while acquiring cartesian interfaces \n");
        dialog_severe_error(GTK_MESSAGE_ERROR,(char *) "Problems acquiring cartesian interface", (char *) "Check if cartesian interface is running", true);
        interfaceError = true;
    }

    if (interfaceError == false)
        {
            fprintf(stderr, "Allocating memory \n");

            int j,k;
            index  = new int [MAX_NUMBER_OF_JOINTS];
            entry_id = new guint [0];
            *entry_id = -1;

            frame_slider1 = new GtkWidget* [MAX_NUMBER_OF_JOINTS];
            sliderArray    = new GtkWidget* [NUMBER_OF_CARTESIAN_COORDINATES];
            currPosArray = new GtkWidget* [NUMBER_OF_CARTESIAN_COORDINATES];

            //fprintf(stderr, "sliderArray has address 0x%x\n", (unsigned int) sliderArray);

            GtkWidget *top_hbox 		 = NULL;
            GtkWidget *bottom_hbox		 = NULL;
            GtkWidget *panel_hbox		 = NULL;

            GtkWidget *inv1 			 = NULL;
            GtkWidget *invArray[NUMBER_OF_CARTESIAN_COORDINATES];

            GtkWidget *homeArray[NUMBER_OF_CARTESIAN_COORDINATES];
            GtkWidget *framesArray[NUMBER_OF_CARTESIAN_COORDINATES];

            GtkWidget *sw				 = NULL;

            //creation of the top_hbox
            top_hbox = gtk_hbox_new (FALSE, 0);
            gtk_container_set_border_width (GTK_CONTAINER (top_hbox), 10);
            gtk_container_add (GTK_CONTAINER (vbox), top_hbox);
		
            inv1 = gtk_fixed_new ();
            gtk_container_add (GTK_CONTAINER (top_hbox), inv1);
		
            Vector o;
            Vector x;
		
            while (!crt->getPose(x,o))
                Time::delay(0.001);
      
            Matrix R = axis2dcm(o);
            Vector eu = dcm2euler(R);
            //x(0) = 1;    x(1) = 1;   x(2) = 1;
            //o(0) = 1;    o(1) = 0;   o(2) = 0;   o(3) = 0;

            char buffer[40] = {'i', 'n', 'i', 't'};
		
            int numberOfRows = 3;
		
            int height, width;
            height = 100;
            width = 180;
            double min,max;

            std::string limitString=partLabel;
            limitString=limitString+"_workspace";
            Bottle bCartesianLimits;
            if (finder->check(limitString.c_str()))
                {
                    //fprintf(stderr, "There seem limits for %s", partLabel);
                    bCartesianLimits = finder->findGroup(limitString.c_str());
                    //fprintf(stderr, "...got: %s", bCartesianLimits.toString().c_str());
                }


            fprintf(stderr, "Starting embedding cartesian GUI widgets \n");
            for (k = 0; k<NUMBER_OF_CARTESIAN_COORDINATES; k++)
                {
                    //fprintf(stderr, "Adding invArray \n");
                    invArray[k] = gtk_fixed_new ();

                    index[k]=k;
                    j = k/numberOfRows;
	  
                    if (k==0)
                        {
                            sprintf(buffer, "x");
                            if (bCartesianLimits.check("xmin") && bCartesianLimits.check("xmax"))
                                {
                                    min = bCartesianLimits.find("xmin").asDouble();
                                    max = bCartesianLimits.find("xmax").asDouble();
                                }
                            else
                                {
                                    min = x(0) - 0.1 * fabs(x(0));
                                    max = x(0) + 0.1 * fabs(x(0));
                                }
                        }
                    if (k==1)
                        {
                            sprintf(buffer, "y");
                            if (bCartesianLimits.check("ymin") && bCartesianLimits.check("ymax"))
                                {
                                    min = bCartesianLimits.find("ymin").asDouble();
                                    max = bCartesianLimits.find("ymax").asDouble();
                                }
                            else
                                {
                                    min = x(1) - 0.1 * fabs(x(1));
                                    max = x(1) + 0.1 * fabs(x(1));
                                }
                        }
                    if (k==2)
                        {
                            sprintf(buffer, "z");
                            if (bCartesianLimits.check("zmin") && bCartesianLimits.check("zmax"))
                                {
                                    min = bCartesianLimits.find("zmin").asDouble();
                                    max = bCartesianLimits.find("zmax").asDouble();
                                }
                            else
                                {
                                    min = x(2) - 0.1 * fabs(x(2));
                                    max = x(2) + 0.1 * fabs(x(2));
                                }
                        }
                    if (k==3)
                        {
                            sprintf(buffer, "euler-alpha");
                            min = -180;
                            max = 180;
                        }
                    if (k==4)
                        {
                            sprintf(buffer, "euler-beta");
                            min = -180;
                            max = 180;
                        }
                    if (k==5)
                        {
                            sprintf(buffer, "euler-gamma");
                            min = -180;
                            max = 180;
                        }

                    frame_slider1[k] = gtk_frame_new ("Value:");
                    //fprintf(stderr, "Initializing sliders %d \n",k);
                    if (min<max)
                        {
                            sliderArray[k]	  =  gtk_hscale_new_with_range(min, max, 1);
                            if (k<3)
                                gtk_scale_set_digits((GtkScale*) sliderArray[k],2);
                            else
                                gtk_scale_set_digits((GtkScale*) sliderArray[k],1);
                        }
                    else
                        {
                            sliderArray[k]    =  gtk_hscale_new_with_range(1, 2, 1);
                            if (k<3)
                                gtk_scale_set_digits((GtkScale*) sliderArray[k],2);
                            else
                                gtk_scale_set_digits((GtkScale*) sliderArray[k],1);
                        }
                    currPosArray[k]   =  gtk_entry_new();

                    //fprintf(stderr, "Initializing the buttons %d \n", k);
                    homeArray[k]		= gtk_button_new_with_mnemonic ("Home");
                    //fprintf(stderr, "Initializing frames %d \n", k);
                    framesArray[k]	= gtk_frame_new (buffer);
		
                    gtk_fixed_put (GTK_FIXED(inv1), invArray[k], 0+(k%numberOfRows)*width, 0+ j*height);	
                    //Positions
                    //fprintf(stderr, "Positioning buttons %d \n", k);
                    gtk_fixed_put	(GTK_FIXED(invArray[k]), frame_slider1[k],  60, 10    );
                    gtk_fixed_put	(GTK_FIXED(invArray[k]), sliderArray[k],    65, 20    );
                    gtk_fixed_put	(GTK_FIXED(invArray[k]), currPosArray[k],   95, 70);
	  
                    int buttonDist= 24;
                    int buttonOffset = 13;
                    gtk_fixed_put	(GTK_FIXED(invArray[k]), homeArray[k],      6, buttonOffset);
                    gtk_fixed_put	(GTK_FIXED(invArray[k]), framesArray[k],    0,  0);
		
                    //Dimensions
                    //fprintf(stderr, "Dimensioning buttons %d \n", k);
                    gtk_widget_set_size_request 	(frame_slider1[k], 110, 50);
                    gtk_widget_set_size_request 	(sliderArray[k], 90, 40);
                    gtk_widget_set_size_request 	(currPosArray[k], 70, 20);
                    gtk_widget_set_size_request 	(homeArray[k], 50, 25);
                    gtk_widget_set_size_request 	(framesArray[k], width, height);
			
                    /*
                     * Positions commands
                     */
                    //fprintf(stderr, "Assinging callback %d \n", k);
                    gtk_range_set_update_policy 	((GtkRange *) (sliderArray[k]), GTK_UPDATE_DISCONTINUOUS);
                    if (k<3)
                        gtk_range_set_value 			((GtkRange *) (sliderArray[k]),  x(k));
                    if (k>=3 && k <= 5)
                        gtk_range_set_value 			((GtkRange *) (sliderArray[k]),  eu(k-3) * 180/M_PI);
	  
                    g_signal_connect (sliderArray[k], "value-changed", G_CALLBACK(position_slider_changed), this);


                }
      
            /*
             * Display current position
             */      
            *entry_id = gtk_timeout_add(UPDATE_TIME, (GtkFunction) display_cartesian_pose, this);
            for (k = 0; k<NUMBER_OF_CARTESIAN_COORDINATES; k++)
                gtk_editable_set_editable ((GtkEditable*) currPosArray[k], FALSE);

            /*
             * Common commands
             */      
            GtkWidget *frame3;
            frame3 = gtk_frame_new ("Commands:");
            gtk_fixed_put	(GTK_FIXED(inv1), frame3,       (NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         (NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(frame3, 180, 240);

            //Button 0 in the panel

            GtkWidget *button0 = gtk_button_new_with_mnemonic ("Open sequence tab");
            gtk_fixed_put (GTK_FIXED (inv1), button0, 10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         20+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request     (button0, 150, 25);
	    fprintf(stderr, "Initializing the table \n");
	    init_cartesian_table();
	    fprintf(stderr, "Connecting the callbacks for the table \n");
            g_signal_connect (button0, "clicked", G_CALLBACK (cartesian_table_open), this);
		      
            //Button1 in the panel
            GtkWidget *button1 = gtk_button_new_with_mnemonic ("Stop");
            gtk_fixed_put (GTK_FIXED (inv1), button1, 10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         45+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(button1, 150, 25);
            //g_signal_connect (button1, "clicked", G_CALLBACK (stop_motion), crt);

            //Velocity
            GtkWidget *frame7;
            frame7 = gtk_frame_new ("Time[sec]:");
            gtk_fixed_put	(GTK_FIXED(inv1), frame7,       5+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,          70 + (NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(frame7, 160, 50);
            sliderVelocity = new GtkWidget;
            sliderVelocity =  gtk_hscale_new_with_range(1, 10, 1);
            gtk_scale_set_digits((GtkScale*) sliderVelocity,2);
            gtk_fixed_put	(GTK_FIXED(inv1), sliderVelocity,    60+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,          80 + (NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(sliderVelocity, 90, 40);
            gtk_range_set_update_policy 	((GtkRange *) (sliderVelocity), GTK_UPDATE_DISCONTINUOUS);
            gtk_range_set_value 			((GtkRange *) (sliderVelocity),  2);


            //Diplay axis
            po = new GtkWidget*[4];
            for (int i=0; i < 4; i++)
                {
                    po[i] = gtk_entry_new();
                }
      
            //Display axis
            GtkWidget *frame5;
            frame5 = gtk_frame_new ("Axis:");
            gtk_fixed_put	(GTK_FIXED(inv1), frame5,       5+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         140 + (NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(frame5, 80, 85);
            gtk_fixed_put	(GTK_FIXED(inv1), po[0],   10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         155+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_fixed_put	(GTK_FIXED(inv1), po[1],   10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         175+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_fixed_put	(GTK_FIXED(inv1), po[2],   10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         195+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);

            //Display angle
            GtkWidget *frame6;
            frame6 = gtk_frame_new ("Angle:");
            gtk_fixed_put	(GTK_FIXED(inv1), frame6,       85+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         155 + (NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(frame6, 80, 45);
            gtk_fixed_put	(GTK_FIXED(inv1), po[3],   90+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         175+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);

            for (int i=0; i < 4; i++)
                {
                    gtk_widget_set_size_request 	(po[i], 70, 20);
                    gtk_editable_set_editable ((GtkEditable*) po[i], FALSE);
                }

            *entry_id = gtk_timeout_add(UPDATE_TIME, (GtkFunction) display_axis_pose, this);

            //CheckButton in the panel
            GtkWidget *check= gtk_check_button_new_with_mnemonic ("Tracking Mode");
            gtk_fixed_put (GTK_FIXED (inv1), check, 10+(NUMBER_OF_CARTESIAN_COORDINATES%numberOfRows)*width,         120+(NUMBER_OF_CARTESIAN_COORDINATES/numberOfRows)*height);
            gtk_widget_set_size_request 	(check, 150, 25);
            //g_signal_connect (check, "clicked", G_CALLBACK (toggle_tracking_mode), crt);
      
        }
}

cartesianMover::~cartesianMover()
{
    fprintf(stderr, "destroying cartesianMover \n");
    delete[] po;
}

void cartesianMover::releaseDriver()
{
    fprintf(stderr, "cartesianMover closing driver...");
    partDd->close();
}
