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

/** 
\defgroup laserScannerGui laserScannerGui
 
A simple GUI to display the distance mesaurements of iKart's laser scanner.
 
Copyright (C) 2010 RobotCub Consortium
 
Author: Marco Randazzo

CopyPolicy: Released under the terms of the GNU GPL v2.0.

\section intro_sec Description
A simple GUI to display the distance mesaurements of iKart's laser scanner.
 
\section lib_sec Libraries 
- YARP libraries. 
- OpenCV libraries.

\section parameters_sec Parameters
None.

\section portsa_sec Ports Accessed
None. 
 
\section portsc_sec Ports Created 
The module creates the port /laserScannerGui:i used to receive the laser data.

\section in_files_sec Input Data Files
None.

\section out_data_sec Output Data Files 
None. 
 
\section conf_file_sec Configuration Files
None.
 
\section tested_os_sec Tested OS
Windows, Linux

\author Marco Randazzo
*/ 

#include<iostream>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<string>
#include<stdio.h>

#define _USE_MATH_DEFINES
#include<math.h>


#include <yarp/dev/Drivers.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/ILaserRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

double scale =100; //global scale factor 
double robot_radius = 715.0/2.0/1000.0; //m
double laser_position = 245.0/1000.0; //m
bool verbose = false;
bool absolute = false;
bool compass  = true;
int rate=50; //ms


#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif



int main(int argc, char *argv[])
{
    Network yarp;

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
    lasOptions.put("device", "laserRangefinder2DClient");
    lasOptions.put("local",  "/laserScannerGui/laser:i");
    lasOptions.put("remote", "/ikart/laser:o");
    lasOptions.put("period", "10");
    bool b = drv->open(lasOptions);
    if (!b)
    {
        yError() << "Unable to open polydriver";
        return 0;
    }
    yarp::dev::ILaserRangefinder2D* iLas = 0;
    drv->view(iLas);
    if (iLas == 0)
    {
        yError() << "Unable to ILaserRangefinder2D interface";
        return 0;
    }

    double angle_min = 0;
    double angle_max = 0;
    double angle_step = 0;
    iLas->getScanAngle(angle_min, angle_max);
    double angle_tot = (angle_max - angle_min);
    iLas->getAngularStep(angle_step);
    int scans = (int)(angle_tot / angle_step);

    yarp::sig::Vector laser_data;
    iLas->getMeasurementData(laser_data);
    int laser_data_size = laser_data.size();
    if (laser_data_size != scans) { yWarning() << "Problem detected in size of laser measurement vector"; }

    
}
