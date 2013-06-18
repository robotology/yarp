/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#if defined(WIN32)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <gtkmm.h>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/ConstString.h>
#include "main_window.h"


using namespace std;

#define HELP_MESSAGE        "\
Usage:\n\
      gymanager [option...]\n\n\
Options:\n\
  --help                  Show help\n\
  --from                  Configuration file name\n"

#define DEF_CONFIG_FILE     "ymanager.ini"


void onSignal(int signum);


#if defined(WIN32)
#include <yarp/os/impl/PlatformSignal.h>
#include <windows.h>
/*
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
*/

#else

#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#endif

int main(int __argc, char *__argv[]) 
{

    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose(false);
//    rf.setDefaultContext("");
    rf.setDefaultConfigFile(DEF_CONFIG_FILE);
    rf.configure(__argc, __argv);

    yarp::os::Network yarp;
    yarp.setVerbosity(-1);

    yarp::os::Property config;
    config.fromString(rf.toString());

    if(config.check("help"))
    {
        cout<<HELP_MESSAGE<<endl;
        return 0;
    }
    
    /**
     *  preparing default options
     */
    
    std::string inifile=rf.findFile("from").c_str();
    std::string inipath="";
    size_t lastSlash=inifile.rfind("/");
    if (lastSlash!=std::string::npos)
        inipath=inifile.substr(0, lastSlash+1);
    else
    {
        lastSlash=inifile.rfind("\\");
        if (lastSlash!=std::string::npos)
            inipath=inifile.substr(0, lastSlash+1);
    }
    
    if(!config.check("ymanagerini_dir"))
        config.put("ymanagerini_dir", inipath.c_str());
    
    if(!config.check("apppath"))
        config.put("apppath", "./");

    if(!config.check("modpath"))
        config.put("modpath", "./");
    
    if(!config.check("respath"))
        config.put("respath", "./");

    if(!config.check("load_subfolders"))
        config.put("load_subfolders", "no");
    
    if(!config.check("watchdog"))
        config.put("watchdog", "no");

    if(!config.check("module_failure"))
        config.put("module_failure", "prompt");
    
    if(!config.check("connection_failure"))
        config.put("connection_failure", "prompt");

    if(!config.check("auto_connect"))
        config.put("auto_connect", "no");
    
    if(!config.check("auto_dependency"))
        config.put("auto_dependency", "no");


#if defined(WIN32)
    //setup signal handler for windows
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) onSignal);
    ACE_OS::signal(SIGBREAK, (ACE_SignalHandler) onSignal);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) onSignal);

#else
    // Set up the structure to specify the new action.
    struct sigaction new_action, old_action;     
    new_action.sa_handler = onSignal;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGINT, &new_action, NULL);
    sigaction (SIGHUP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGHUP, &new_action, NULL);
    sigaction (SIGTERM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGTERM, &new_action, NULL);
#endif

    if(!Glib::thread_supported())
        Glib::thread_init();
    gdk_threads_init();
    GDK_THREADS_ENTER();
    Gtk::Main kit(__argc, __argv);
    MainWindow window(config);
    Gtk::Main::run(window);
    GDK_THREADS_LEAVE();
    return 0;
}

void onSignal(int signum)
{
    cout<<"Use <quit> menu to exit!"<<endl;
}

