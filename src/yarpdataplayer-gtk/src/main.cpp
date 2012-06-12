/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Vadim Tikhanoff and Ali Paikan
 * email:  vadim.tikhanoff@iit.it ali.paikan@iit.it
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
\defgroup dataSetPlayer dataSetPlayer

@ingroup icub_tools

A module that reproduces in a synchronized way, previously acquired data (using dataDumper) from the iCub or other source of input.

Copyright (C) 2010 RobotCub Consortium

Author: Vadim Tikhanoff and Ali Paikan

CopyPolicy: Released under the terms of the GNU GPL v2.0.

\section intro_sec Description

This module uses gtkmm to provide the user a simple but informative user-interface in order to manipulate the previously acquired data.
When playing it uses the timestamps provided from the time of aquisition to synchronize the sending of the data as if the robot was present.

\section lib_sec Libraries
- YARP libraries.
- ICUB libraries
- OpenCV libraries.
- GTKMM libraries.

\section running
dataSetPlayer can run with or without the GUI (for server use and commands are sent via the rpc port)
For gui run normally ./dataSetPlayer or dataSetPlayer.exe
Without GUI run with extra parameter hidden: ./dataSetPlayer hidden or dataSetPlayer.exe hidden

\section dataDumper file example
dataDumper data.log file example:\n

  Type: Bottle; \n
  Source: /icub/head/state:o\n
  9566    1324373535.040288   -2.32967 0.043956 1.450549 -0.56044 1.704894 4.136408\n
  etc...\n

 type: is used to identify what kind of data the player is required to send
 Source: is used to initially set up the ports of the player. This can be changed using the GUI.

\section parameters_sec Parameters
--name \e modName
- The parameter \e modName identifies the stem-name of the open
  ports.

\section portsc_sec Ports Created

- \e/<modName>/rpc remote procedure call.\n
    Recognized remote commands: \n
    - [help]  : returns the list of available rpc commands.\n
    - [get]   : get requests \n
        * [part]:the system returns the requested element. <part> is one of the parts loaded \n
    - [set]   : set requests \n
        * [part] [frameNumber] the part will be set to the desired frame. (currently all parts will be set to the same frame number) \n
    - [step]  : the player will be stepped until all parts have sent data. \n
    - [quit]  : stops the player \n
    - [load]  : load request \n
        * [path] the path name of the folder
    - [play]  : plays the loaded files
    - [pause] : pauses the loaded files
    - [stop]  : stops the loaded files

- \e The ports belonging to each of the parts are dynamically created and can be changed using the GUI.

\section in_files_sec Input Data Files
The player will look, in a recursive way, into directories in order to create the parts needed and retreive the data
The data name is the default dataDumper name: data.log

An example directory tree containing data (data.log) can be:
\code
                /head/data.log 
                /torso/data.log 
                /images/leftCamera/data.log 
/experiment1/ 
                /images/rightCamera/data.log 
                /left_leg/data.log 
                /right_leg/data.log 
\endcode

If the directory indicated is either "experiment1" or within experiment, the player will successfully load all required data.
The parts name will be taken from each subdirectory of the /experiment1 forder.

\note Currently, if the directory selected is (in this example) head, torso etc, the player will not load the files as it will not be able to find subdirs.

\section out_data_sec Output Data Files
The player will send throught the ports all the data in a synchronised way.

\todo 
 - main slider bar selection. This needs an on-click and on-release event in order not to interfere with update and playback
 - when the search for a directory has not provided anything (user selected directory containing only *.log files)

\section tested_os_sec Tested OS
Windows, Linux

\author Vadim Tikhanoff
*/

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <gtkmm/main.h>
#include "iCub/main_window.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#if defined(WIN32)
    #include <windows.h>
#else
    #include <errno.h>
    #include <sys/types.h>
    #include <signal.h>
#endif

using namespace std;
using namespace yarp::os;

int main(int argc, char *argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        fprintf(stderr, "Sorry YARP network does not seem to be available, is the yarp server available?\n");
        return -1;
    }
    if(!Glib::thread_supported())
        Glib::thread_init();
    yarp::os::ResourceFinder rf;
    rf.setVerbose( true );
    rf.setDefaultConfigFile( "config.ini" );        //overridden by --from parameter
    rf.setDefaultContext( "dataSetPlayer/conf" );   //overridden by --context parameter
    rf.configure( "ICUB_ROOT", argc, argv );
    gdk_threads_init();
    GDK_THREADS_ENTER();
    Gtk::Main kit(argc, argv);
    MainWindow window(rf);
    
    if (rf.check("hidden"))
    {
        char env[] = "export DISPLAY=:0";
        putenv(env);
        fprintf(stdout,"window should be now hidden\n");
        Gtk::Main::run();
        //unsetenv("DISPLAY=:0");
    }
    else
    {
        fprintf(stdout,"window should be now visible\n");
        Gtk::Main::run(window);
    }

    GDK_THREADS_LEAVE();
    return 0;
}
