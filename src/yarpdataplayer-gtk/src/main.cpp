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
@ingroup icub_guis
@ingroup icub_tools

A module that reproduces in a synchronized way, previously acquired data (using \ref dataDumper) from the iCub or other source of input.

\section intro_sec Description

This module uses gtkmm to provide the user a simple but informative user-interface in order to manipulate the previously acquired data.
When playing it uses the timestamps provided from the time of aquisition to synchronize the sending of the data as if the robot was present.

\image html datasetplayer.jpg
\image latex datasetplayer.eps "The dataSetPlayer GUI running on Windows" width=6cm
 
\section lib_sec Libraries
- YARP libraries.
- ICUB libraries
- OpenCV libraries.
- GTKMM libraries.

\section running_sec Running
dataSetPlayer can run with or without the GUI (for server use and commands are sent via the rpc port)
For gui run normally \e dataSetPlayer .
Without GUI run with the extra parameter \e hidden: 

\verbatim
  dataSetPlayer --hidden 
\endverbatim

\section file-sec dataDumper file example
dataDumper data.log file example:

\verbatim
  9566    1324373535.040288   -2.32967 0.043956 1.450549 -0.56044 1.704894 4.136408
  etc...
\endverbatim

dataDumper info.log file example:

\verbatim
  Type: Bottle;  
  [1324373535.040288] /icub/head/state:o [connected]  
  etc...
\endverbatim
    
\e Type: is used to identify what kind of data the player is    
   required to send. 
 
\e The names of the ports open up by the player are initially    
   set up based on the content of info.log file. This can be 
   then changed using the GUI. 

\section parameters_sec Parameters

--hidden
- run with or without gui
 
--withExtraTimeCol index
- loads the log files created by the datadumper with both rx and tx time.
  The user must select which timestamp to use (txTime index = 1 or rxTime index = 2)


--name \e modName
- The parameter \e modName identifies the stem-name of the open
  ports.

 \section portsif_sec Ports Interface
 The interface to this module is implemented through
 \ref dataSetPlayer_IDL . \n

- The ports belonging to each of the parts are dynamically created and can be changed using the GUI.

\section in_files_sec Input Data Files
The player will look, in a recursive way, into directories in order to create the parts needed and retreive the data.

The data name is the default \ref dataDumper names: data.log and    
info.log.    

An example directory tree containing data (data.log+info.log)    
can be:    
\code
/experiment1/
             /head/data.log;info.log
             /torso/data.log;info.log;
             /images/leftCamera/data.log;info.log
             /images/rightCamera/data.log;info.log
             /left_leg/data.log;info.log
             /right_leg/data.log;info.log
\endcode

If the directory indicated is either "experiment1" or within experiment, the player will successfully load all required data.

The parts name will be taken from each subdirectory of the /experiment1 forder.

\note Currently, if the directory selected is (in this example) head, torso etc, the player will not load the files as it will not be able to find subdirs.

\todo
 - main slider bar selection. This needs an on-click and on-release event in order not to interfere with update and playback
 - when the search for a directory has not provided anything (user selected directory containing only *.log files)

\section tested_os_sec Tested OS
Windows, Linux

\sa dataDumper
\sa dataSetPlayerExample

Copyright (C) 2010 RobotCub Consortium

\Author: Vadim Tikhanoff and Ali Paikan

CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <gtkmm.h>
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
    rf.setDefaultContext( "dataSetPlayer" );        //overridden by --context parameter
    rf.configure( argc, argv );
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



