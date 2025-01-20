/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "mainwindow.h"
#include <QApplication>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Os.h>
#include <yarp/os/Log.h>
#include <yarp/os/ResourceFinder.h>
#include <string>
#include <yarp/os/ResourceFinderOptions.h>


#define HELP_MESSAGE        "\
Usage:\n\
   yarpmanager [option]\n\n\
Options:\n\
  --from                Configuration file name\n\
  --application         Path to application to open\n\
  --ymanagerini_dir     \n\
  --apppath             \n\
  --modpath             \n\
  --respath             \n\
  --templpath           \n\
  --load_subfolders     \n\
  --watchdog            \n\
  --module_failure      \n\
  --connection_failure  \n\
  --auto_connect        \n\
  --auto_dependency     \n\
  --add_current_dir     add the current dir to the search path\n\
"

#define DEF_CONFIG_FILE     "ymanager.ini"


#if defined(_WIN32)
//#include <csignal>
#include <windows.h>

#else

#include <cerrno>
#include <sys/types.h>
#include <csignal>
#endif

void onSignal(int signum);

int main(int argc, char *argv[])
{

#if defined(_WIN32)
    // We create a console. This is inherited by console processes created by the localhost broker.
    // It is useful because new processes can receive ctrl+brk signals and shutdown cleanly.
    // This console is not actually needed for printing so we hide it.  In principle we could
    // redirect the output of all processes to this console, in practice this would be end up
    // soon in a big mess.
   AllocConsole();
   HWND hwnd = GetConsoleWindow();
   HWND hide = FindWindowA("ConsoleWindowClass",NULL);
   ShowWindow(hide, 0);
#endif
    QApplication a(argc, argv);

    // Setup resource finder

    yarp::os::ResourceFinder& rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
    rf.setDefaultContext("yarpmanager");
    rf.setDefaultConfigFile(DEF_CONFIG_FILE);
    rf.configure(argc, argv);

    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    yarp::os::Property config;
    config.fromString(rf.toString());

    if(config.check("help"))
    {
        yInfo("%s",HELP_MESSAGE);
        return 0;
    }

    /**
    *  preparing default options
    */
    bool add_curr_dir = false;
    if(config.check("add_current_dir"))
    {
        add_curr_dir=true;
    }
    const int cur_dir_max_size=512;
    char current_dir[cur_dir_max_size]; current_dir[0]=0;
    yarp::os::getcwd(current_dir,cur_dir_max_size);
    config.put("current_dir", current_dir);

    std::string inifile=rf.findFile("from");
    std::string inipath;
    size_t lastSlash=inifile.rfind('/');
    if (lastSlash!=std::string::npos){
        inipath=inifile.substr(0, lastSlash+1);
    }else{
        lastSlash=inifile.rfind('\\');
        if (lastSlash!=std::string::npos){
            inipath=inifile.substr(0, lastSlash+1);
        }
    }

    if(!config.check("ymanagerini_dir")){
        config.put("ymanagerini_dir", inipath);
    }

    yarp::os::Bottle appPaths;
    if(!config.check("apppath")){

        appPaths= rf.findPaths("applications");

        yarp::os::ResourceFinderOptions findRobotScripts;
        findRobotScripts.searchLocations=yarp::os::ResourceFinderOptions::Robot;
        yarp::os::Bottle appPaths2=rf.findPaths("scripts", findRobotScripts);
//        yarp::os::Bottle appPaths2=rf.findPaths("scripts");
//        std::cout << "app path : " << appPaths.toString()<< std::endl;
        QString appPathsStr="";
        for (size_t ind=0; ind < appPaths.size(); ++ind){
            appPathsStr += (appPaths.get(ind).asString() + ";").c_str();
        }
        for (size_t ind=0; ind < appPaths2.size(); ++ind){
            appPathsStr += (appPaths2.get(ind).asString() + ";").c_str();
        }
        if (add_curr_dir)
        {
            appPathsStr += (current_dir + std::string(";")).c_str();
        }
        std::string sss= appPathsStr.toLatin1().data();
        config.put("apppath", appPathsStr.toLatin1().data());
    }

    if(!config.check("modpath")){
       appPaths=rf.findPaths("modules");
       //std::cout << "mod path : " << appPaths.toString()<< std::endl;
       QString modPathsStr="";
       for (size_t ind=0; ind < appPaths.size(); ++ind){
           modPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       if (add_curr_dir)
       {
           modPathsStr += (current_dir + std::string(";")).c_str();
       }
       config.put("modpath", modPathsStr.toLatin1().data());
    }

    if(!config.check("respath")){
       appPaths=rf.findPaths("resources");
       //std::cout << "res path : " << appPaths.toString()<< std::endl;
       QString resPathsStr="";
       for (size_t ind=0; ind < appPaths.size(); ++ind){
           resPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       if (add_curr_dir)
       {
           resPathsStr += (current_dir + std::string(";")).c_str();
       }
       config.put("respath", resPathsStr.toLatin1().data());
    }

    if(!config.check("templpath")){
       appPaths=rf.findPaths("templates/applications");
      // std::cout << "templ path : " << appPaths.toString()<< std::endl;
       QString templPathsStr="";
       for (size_t ind=0; ind < appPaths.size(); ++ind){
            templPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       if (add_curr_dir)
       {
           templPathsStr += (current_dir + std::string(";")).c_str();
       }
       config.put("templpath", templPathsStr.toLatin1().data());

    }

    if(!config.check("load_subfolders")){
        config.put("load_subfolders", "yes");
    }

    if(!config.check("watchdog")){
        config.put("watchdog", "no");
    }

    if(!config.check("module_failure")){
        config.put("module_failure", "prompt");
    }

    if(!config.check("connection_failure")){
        config.put("connection_failure", "prompt");
    }

    if(!config.check("auto_connect")){
        config.put("auto_connect", "no");
    }

    if(!config.check("auto_dependency")){
        config.put("auto_dependency", "no");
    }

#if defined(_WIN32)
    //setup signal handler for windows
//    signal(SIGINT, onSignal);
//    signal(SIGBREAK, onSignal);
//    signal(SIGTERM, onSignal);

#else
    // Set up the structure to specify the new action.
//     struct sigaction new_action, old_action;
//     new_action.sa_handler = onSignal;
//     sigemptyset (&new_action.sa_mask);
//     new_action.sa_flags = 0;
//     sigaction (SIGINT, NULL, &old_action);
//     if (old_action.sa_handler != SIG_IGN)
//         sigaction (SIGINT, &new_action, NULL);
//     sigaction (SIGHUP, NULL, &old_action);
//     if (old_action.sa_handler != SIG_IGN)
//         sigaction (SIGHUP, &new_action, NULL);
//     sigaction (SIGTERM, NULL, &old_action);
//     if (old_action.sa_handler != SIG_IGN)
//         sigaction (SIGTERM, &new_action, NULL);
#endif

    MainWindow w;
    w.init(config);
    w.show();

    return a.exec();
}

// void onSignal(int signum)
// {
//     qDebug("Use <quit> menu to exit!");
// }
