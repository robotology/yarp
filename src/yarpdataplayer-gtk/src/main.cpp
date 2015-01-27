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


#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <gtkmm.h>
#include "main_window.h"
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
