/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
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


#define _USE_MATH_DEFINES

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <limits>
#include <cmath>

#include <cv.h>
#include <highgui.h>
#include <vector>

#include <yarp/dev/Drivers.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

CvFont font;
CvFont fontBig;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

const CvScalar color_bwhite = cvScalar(200,200,255);
const CvScalar color_white  = cvScalar(255,255,255);
const CvScalar color_red    = cvScalar(0,0,255);
const CvScalar color_yellow = cvScalar(0,255,255);
const CvScalar color_black  = cvScalar(0,0,0);
const CvScalar color_gray   = cvScalar(100,100,100);

#define ASPECT_LINE  0
#define ASPECT_POINT 1

void drawGrid(IplImage *img, double scale)
{
    cvLine(img,cvPoint(0,0),cvPoint(img->width,img->height),color_black);
    cvLine(img,cvPoint(img->width,0),cvPoint(0,img->height),color_black);
    cvLine(img,cvPoint(img->width/2,0),cvPoint(img->width/2,img->height),color_black);
    cvLine(img,cvPoint(0,img->height/2),cvPoint(img->width,img->height/2),color_black);
    const int step = (int)(0.5 * scale); //mm
/*
    for (int xi=0; xi<img->width; xi+=step)
        cvLine(img,cvPoint(xi,0),cvPoint(xi,img->height),color_black);
    for (int yi=0; yi<img->height; yi+=step)
        cvLine(img,cvPoint(0,yi),cvPoint(img->width,yi),color_black);
*/
    char buff [10];
    int  rad_step=0;
    if   (scale>60)
        rad_step=1;
    else
        rad_step=2;
    for (int rad=0; rad<20; rad+=rad_step)
    {
        sprintf (buff,"%3.1fm",float(rad)/2);
        cvPutText(img, buff, cvPoint(img->width/2,int(float(img->height)/2.0-float(step)*rad)), &font, cvScalar(0, 0, 0, 0));
        cvCircle(img,cvPoint(img->width/2,img->height/2),step*rad,color_black);
    }

}

void drawRobot (IplImage *img, double robot_radius, double scale)
{
    cvRectangle(img,cvPoint(0,0),cvPoint(img->width,img->height),cvScalar(0,0,0),CV_FILLED);

    //draw a circle
    double v1 = robot_radius*scale; if (v1 < 0) v1 = 0;
    double v2 = robot_radius*scale - 1; if (v2 < 0) v2 = 0;
    double v3 = robot_radius*scale - 2; if (v3 < 0) v3 = 0;

    cvCircle(img,cvPoint(img->width/2,img->height/2),(int)(v1),color_gray,CV_FILLED);
    cvCircle(img,cvPoint(img->width/2,img->height/2),(int)(v2),color_black);
    cvCircle(img,cvPoint(img->width/2,img->height/2),(int)(v3),color_black);
}

void drawCompass(const yarp::sig::Vector *comp, IplImage *img, bool absolute)
{
    int sx = 0;
    int sy = 0;
    int ex = 0;
    int ey = 0;
    int tx = 0;
    int ty = 0;
    char buff [20];
    cvCircle(img,cvPoint(img->width/2,img->height/2),250,color_black);
    for (int i=0; i<360; i+=10)
    {
        double ang;
        if  (absolute) ang = i+180;
        else           ang = i+(*comp)[0]+180;
        sx = int(-250*sin(ang/180.0*M_PI)+img->width/2);
        sy = int(250*cos(ang/180.0*M_PI)+img->height/2);
        ex = int(-260*sin(ang/180.0*M_PI)+img->width/2);
        ey = int(260*cos(ang/180.0*M_PI)+img->height/2);
        tx = int(-275*sin(ang/180.0*M_PI)+img->width/2);
        ty = int(275*cos(ang/180.0*M_PI)+img->height/2);
        cvLine(img,cvPoint(sx,sy),cvPoint(ex,ey),color_black);
        CvSize tempSize;
        int lw;
        if      (i==0)     {sprintf(buff,"N");    cvGetTextSize( buff, &fontBig, &tempSize, &lw ); cvPutText(img, buff, cvPoint(tx-tempSize.width/2,ty+tempSize.height/2), &fontBig, cvScalar(0, 0, 0, 0));}
        else if (i==90)    {sprintf(buff,"E");    cvGetTextSize( buff, &fontBig, &tempSize, &lw ); cvPutText(img, buff, cvPoint(tx-tempSize.width/2,ty+tempSize.height/2), &fontBig, cvScalar(0, 0, 0, 0));}
        else if (i==180)   {sprintf(buff,"S");    cvGetTextSize( buff, &fontBig, &tempSize, &lw ); cvPutText(img, buff, cvPoint(tx-tempSize.width/2,ty+tempSize.height/2), &fontBig, cvScalar(0, 0, 0, 0));}
        else if (i==270)   {sprintf(buff,"W");    cvGetTextSize( buff, &fontBig, &tempSize, &lw ); cvPutText(img, buff, cvPoint(tx-tempSize.width/2,ty+tempSize.height/2), &fontBig, cvScalar(0, 0, 0, 0));}
        else               {sprintf(buff,"%d",i); cvGetTextSize( buff, &font   , &tempSize, &lw ); cvPutText(img, buff, cvPoint(tx-tempSize.width/2,ty+tempSize.height/2), &font, cvScalar(0, 0, 0, 0));}
    }
}

void drawNav(const yarp::os::Bottle *display, IplImage *img, double scale)
{
    if (display->size()==8)
    {
        yError ("wrong image format!");
        return;
    }
    double c0 = display->get(0).asDouble();
//     double c1 = display->get(1).asDouble();
//     double c2 = display->get(2).asDouble();
    double angle_f = display->get(3).asDouble();
//     double angle_t = display->get(4).asDouble();
//     double w_f = display->get(5).asDouble();
//     double w_t = display->get(6).asDouble();
    double max_obs_dist = display->get(7).asDouble();
    double angle_g = display->get(8).asDouble();

    CvPoint center;
    center.x = (int)(img->width/2  );
    center.y = (int)(img->height/2 );

    CvPoint ray;
    ray.x=int(200*sin(DEG2RAD*angle_f));
    ray.y=-int(200*cos(DEG2RAD*angle_f));
    ray.x += center.x;
    ray.y += center.y;

    int thickness = 3;
    cvLine(img,center,ray,color_bwhite,thickness);

    ray.x=int(100*sin(DEG2RAD*c0));
    ray.y=-int(100*cos(DEG2RAD*c0));
    ray.x += center.x;
    ray.y += center.y;
    cvLine(img,center,ray,color_red,thickness);

    ray.x=int(150*sin(DEG2RAD*angle_g));
    ray.y=-int(150*cos(DEG2RAD*angle_g));
    ray.x += center.x;
    ray.y += center.y;
    cvLine(img,center,ray,color_yellow,thickness);

    cvCircle(img,cvPoint(img->width/2,img->height/2),(int)(max_obs_dist*scale-1),color_black);
}

void drawLaser(const Vector *comp, vector<yarp::dev::LaserMeasurementData> *las, vector<yarp::dev::LaserMeasurementData> *lmap, IplImage *img, double angle_tot, int scans, double sens_position_x, double sens_position_y, double sens_position_t, double scale, bool absolute, bool verbose, int aspect)
{
    cvZero(img);
    cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, img->height), cvScalar(255, 0, 0), -1);
    CvPoint center;

    double center_angle=sens_position_t;
    if (!absolute) center_angle = 0;
    else center_angle = -180 - (*comp)[0];
    center.x = (int)(img->width / 2 + (sens_position_x*scale)*sin(center_angle / 180 * M_PI));
    center.y = (int)(img->height / 2 - (sens_position_y*scale)*cos(center_angle / 180 * M_PI));

    static double old_time = 0;

    if (las==NULL || comp==NULL)
    {
        return;
    }

    double curr_time = yarp::os::Time::now();
    if (verbose) yError("received vector size:%d ", int(las->size()));
    static int timeout_count = 0;
    if (curr_time - old_time > 0.40) timeout_count++;
    if (verbose) yWarning("time:%f timeout:%d\n", curr_time - old_time, timeout_count);
    old_time = curr_time;
    for (int i = 0; i<scans; i++)
    {
        double x = 0;
        double y = 0;
        (*las)[i].get_cartesian(x, y);
        if (x == std::numeric_limits<double>::infinity() ||
            y == std::numeric_limits<double>::infinity()) continue;

        //if (length<0)     length = 0;
        //else if (length>15)    length = 15; //15m maximum

        //the following rotation is performed to have x axis aligned with screen vertical
        CvPoint ray;
        ray.x = int(-y*scale);
        ray.y = int(-x*scale);
        ray.x += center.x;
        ray.y += center.y;

        int thickness = 2;
        //draw a line
        if (aspect == ASPECT_LINE)
        {
            cvLine(img, center, ray, color_white, thickness);
        }
        else if (aspect == ASPECT_POINT)
        {
            cvLine(img, ray, ray, color_white, 3);
        }

        if (lmap)
        {
            double x = 0;
            double y = 0;
            (*lmap)[i].get_cartesian(x, y);
            CvPoint ray2;
            ray2.x = -int(x*scale);
            ray2.y = -int(y*scale);
            ray2.x += (center.x - int((sens_position_x*scale)*sin(center_angle / 180 * M_PI)));
            ray2.y += (center.y + int((sens_position_y*scale)*cos(center_angle / 180 * M_PI)));
            cvLine(img, center, ray2, color_bwhite, thickness);
        }
    }
}

void display_help()
{
    yInfo() << "Available options:";
    yInfo() << "--scale <double> zoom factor (default 100)";
    yInfo() << "--robot_radius <double> the radius of the disalayed robot footprint ";
    yInfo() << "--sens_position_x <double> the position in meters of the laser center respect to the center of the robot (default 0 m)";
    yInfo() << "--sens_position_y <double> the position in meters of the laser center respect to the center of the robot (default 0 m)";
    yInfo() << "--sens_position_theta <double> the orientation in degrees of the laser sensor respect to the center of the robot (default 0 deg)";
    yInfo() << "--verbose <bool> toggles debug messages on/off (default false)";
    yInfo() << "--absolute <bool> display the laser in absolute o relative mode (default false)";
    yInfo() << "--compass <bool> displays the compass (default true) ";
    yInfo() << "--period <double> the refresh period (default 50 ms)";
    yInfo() << "--aspect <0/1> draws line/points (default 0=lines)";
    yInfo() << "--sens_port <string> the name of the port used by Rangefinder2DClient to connect to the laser device. (mandatory)";
    yInfo() << "";
    yInfo() << "Available commands (pressing the key during execution):";
    yInfo() << "c ...... enables/disables compass.";
    yInfo() << "a ...... set absolute/relative mode.";
    yInfo() << "w ...... zoom in.";
    yInfo() << "s ...... zoom out.";
    yInfo() << "v ...... set verbose mode on/off.";
    yInfo() << "r ...... set refresh period (50/100/200ms).";
    yInfo() << "b ...... change aspect (lines/points)";
    cout << "prova";
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    ResourceFinder rf;

    //retrieve information for the list of parts
    rf.setVerbose();
    rf.setDefaultConfigFile("yarplaserscannergui.ini");
    rf.configure(argc, argv);
    if (rf.check("help"))
    {
        display_help();
        return 0;
    }
    double scale = rf.check("scale", Value(100), "global scale factor").asDouble();
    double robot_radius = rf.check("robot_radius", Value(0.001), "robot radius [m]").asDouble();
    double sens_position_x = rf.check("sens_position_x", Value(0), "sens_position_x [m]").asDouble();
    double sens_position_y = rf.check("sens_position_y", Value(0), "sens_position_y [m]").asDouble();
    double sens_position_t = rf.check("sens_position_theta", Value(0), "sens_position_theta [m]").asDouble();
    bool verbose = rf.check("verbose", Value(false), "verbose [0/1]").asBool();
    bool absolute = rf.check("absolute", Value(false), "absolute [0/1]").asBool();
    bool compass = rf.check("compass", Value(true), "compass [0/1]").asBool();
    int period = rf.check("period",Value(50),"period [ms]").asInt(); //ms
    int aspect = rf.check("aspect", Value(0), "0 draw lines, 1 draw points").asInt();
    string laserport = rf.check("sens_port", Value("/laser:o"), "laser port name").asString();

    string laser_map_port_name;
    laser_map_port_name = "/laserScannerGui/laser_map:i";
    string compass_port_name;
    compass_port_name = "/laserScannerGui/compass:i";
    string nav_display;
    nav_display = "/laserScannerGui/nav_display:i";

    int width = 600;
    int height = 600;

    yarp::dev::PolyDriver* drv = new yarp::dev::PolyDriver;
    Property   lasOptions;
    lasOptions.put("device", "Rangefinder2DClient");
    lasOptions.put("local", "/laserScannerGui/laser:i");
    lasOptions.put("remote", laserport);
    lasOptions.put("period", "10");
    bool b = drv->open(lasOptions);
    if (!b)
    {
        yError() << "Unable to open polydriver";
        delete drv;
        return 0;
    }
    yarp::dev::IRangefinder2D* iLas = 0;
    drv->view(iLas);
    if (iLas == 0)
    {
        yError() << "Unable to get IRangefinder2D interface";
        delete drv;
        return 0;
    }

    double angle_min = 0;
    double angle_max = 0;
    double angle_step = 0;
    iLas->getScanLimits(angle_min, angle_max);
    double angle_tot = (angle_max - angle_min);
    iLas->getHorizontalResolution(angle_step);
    int scans = (int)(angle_tot / angle_step);
    std::vector<yarp::dev::LaserMeasurementData> laser_data;

    BufferedPort<yarp::os::Bottle> laserMapInPort;
    laserMapInPort.open(laser_map_port_name.c_str());
    BufferedPort<yarp::sig::Vector> compassInPort;
    compassInPort.open(compass_port_name.c_str());
    BufferedPort<yarp::os::Bottle> navDisplayInPort;
    navDisplayInPort.open(nav_display.c_str());

    IplImage *img  = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
    IplImage *img2 = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
    cvNamedWindow("Laser Scanner GUI",CV_WINDOW_AUTOSIZE);
    cvInitFont(&font,    CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, CV_AA);
    cvInitFont(&fontBig, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8, 0, 1, CV_AA);

    bool exit = false;
    yarp::sig::Vector compass_data;
    compass_data.resize(3, 0.0);

    while(!exit)
    {
        void *v = cvGetWindowHandle("Laser Scanner GUI");
        if (v == 0)
        {
            exit = true;
            break;
        }

        if (compass)
        {
            yarp::sig::Vector *cmp = compassInPort.read(false);
            if (cmp) compass_data = *cmp;
        }

        iLas->getLaserMeasurement(laser_data);
        int laser_data_size = laser_data.size();

        /*yarp::os::Bottle *las_map = laserMapInPort.read(false);
        if (las_map)
        {
            for (unsigned int i=0; i<1080; i++)
            {
                Bottle* b = las_map->get(i).asList();
                lasermap_data[i].x = b->get(0).asDouble();
                lasermap_data[i].y = b->get(1).asDouble();
            }
        }*/

        //The drawing functions.
        {
            /*if (las_map)
            {
                if (laser_data_size != scans)
                {
                    drawLaser(&compass_data, &laser_data, lasermap_data, img, angle_tot, scans, sens_position, scale, absolute, verbose, aspect);
                }
                else
                {
                    drawLaser(&compass_data, &laser_data, 0, img, angle_tot, scans, sens_position, scale, absolute, verbose, aspect);
                }
            }
            else*/
            {
                if (laser_data_size != scans)
                {
                    yWarning() << "Problem detected in size of laser measurement vector";
                }
                else
                {
                    drawLaser(&compass_data, &laser_data, 0, img, angle_tot, scans, sens_position_x, sens_position_y, sens_position_t, scale, absolute, verbose, aspect);
                }

            }
            drawRobot(img2,robot_radius, scale);
            drawGrid(img,scale);
            if (compass) drawCompass(&compass_data,img,absolute);

            yarp::os::Bottle *nav_display = navDisplayInPort.read(false);
            if (nav_display)
            {
                drawNav(nav_display,img,scale);
            }

            cvAddWeighted(img, 0.7, img2, 0.3, 0.0, img);
            cvShowImage("Laser Scanner GUI",img);
        }

        SystemClock::delaySystem(double(period)/1000.0+0.005);

        //if ESC is pressed, exit.
        int keypressed = cvWaitKey(2); //wait 2ms. Lower values do not work under Linux
        keypressed &= 0xFF; //this mask is required in Linux systems
        if(keypressed == 27) exit = true;
        if(keypressed == 'w' && scale <500)
        {
            //scale+=0.001;
            scale*=1.02;
            yInfo("scale factor is now:%.3f",scale);
        }
        if(keypressed == 's' && scale >15)
        {
           //scale-=0.001;
           scale/=1.02;
           yInfo("scale factor is now:%.3f", scale);
        }
        if(keypressed == 'v' )
        {
           verbose= (!verbose);
           if (verbose) yInfo("verbose mode is now ON");
           else         yInfo("verbose mode is now OFF");
        }
        if(keypressed == 'a' )
        {
            absolute= (!absolute);
            if (absolute) yInfo("display is now in ABSOLUTE mode");
            else          yInfo("display is now in RELATIVE mode");
        }
        if(keypressed == 'r' )
        {
            if      (period == 0)  period = 50;
            else if (period == 50) period = 100;
            else if (period == 100) period = 200;
            else if (period == 200) period = 0;
            yInfo("refresh period set to %d ms.", period);
        }
        if(keypressed == 'c' )
        {
            compass= (!compass);
            if (compass) { yInfo( "compass is now ON"); }
            else         { yInfo( "compass is now OFF"); compass_data.zero(); }
        }
        if (keypressed == 'b')
        {
            aspect = aspect + 1;
            if (aspect > 1) aspect = 0;
        }
        if(keypressed == 'h' ||
            keypressed == 'H')
        {
            yInfo("available commands:");
            yInfo("c ...... enables/disables compass.");
            yInfo("a ...... set absolute/relative mode.");
            yInfo("w ...... zoom in.");
            yInfo("s ...... zoom out.");
            yInfo("v ...... set verbose mode on/off.");
            yInfo("r ...... set refresh rate.");
            yInfo("b ...... change aspect");
        }
    }

    compassInPort.close();
    laserMapInPort.close();
    navDisplayInPort.close();
    cvDestroyAllWindows();
    cvReleaseImage(&img);
    if (drv) delete drv;
}
