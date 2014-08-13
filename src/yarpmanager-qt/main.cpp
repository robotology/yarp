/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "mainwindow.h"
#include <QApplication>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/ResourceFinderOptions.h>


#define HELP_MESSAGE        "\
Usage:\n\
      gyarpmanager [option...]\n\n\
Options:\n\
  --help                  Show help\n\
  --from                  Configuration file name\n\
  --application           Path to application to open\n"

#define DEF_CONFIG_FILE     "ymanager.ini"


#if defined(WIN32)
//#include <yarp/os/impl/PlatformSignal.h>
#include <windows.h>

#else

#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#endif

void onSignal(int signum);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Setup resource finder

    yarp::os::ResourceFinder rf;
    rf.setVerbose(false);
    //    rf.setDefaultContext("");
    rf.setDefaultConfigFile(DEF_CONFIG_FILE);
    rf.configure(argc, argv);

    yarp::os::Network yarp;
    yarp.setVerbosity(-1);

    yarp::os::Property config;
    config.fromString(rf.toString());

    if(config.check("help")){
        qDebug("%s",HELP_MESSAGE);
        return 0;
    }

    /**
    *  preparing default options
    */

    std::string inifile=rf.findFile("from").c_str();
    std::string inipath="";
    size_t lastSlash=inifile.rfind("/");
    if (lastSlash!=std::string::npos){
        inipath=inifile.substr(0, lastSlash+1);
    }else{
        lastSlash=inifile.rfind("\\");
        if (lastSlash!=std::string::npos){
            inipath=inifile.substr(0, lastSlash+1);
        }
    }

    if(!config.check("ymanagerini_dir")){
        config.put("ymanagerini_dir", inipath.c_str());
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
        for (int ind=0; ind < appPaths.size(); ++ind){
            appPathsStr += (appPaths.get(ind).asString() + ";").c_str();
        }
        for (int ind=0; ind < appPaths2.size(); ++ind){
            appPathsStr += (appPaths2.get(ind).asString() + ";").c_str();
        }
        config.put("apppath", appPathsStr.toLatin1().data());
    }

    if(!config.check("modpath")){
       appPaths=rf.findPaths("modules");
       //std::cout << "mod path : " << appPaths.toString()<< std::endl;
       QString modPathsStr="";
       for (int ind=0; ind < appPaths.size(); ++ind){
           modPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       config.put("modpath", modPathsStr.toLatin1().data());
    }

    if(!config.check("respath")){
       appPaths=rf.findPaths("resources");
       //std::cout << "res path : " << appPaths.toString()<< std::endl;
       QString resPathsStr="";
       for (int ind=0; ind < appPaths.size(); ++ind){
           resPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       config.put("respath", resPathsStr.toLatin1().data());
    }

    if(!config.check("templpath")){
       appPaths=rf.findPaths("templates/applications");
      // std::cout << "templ path : " << appPaths.toString()<< std::endl;
       QString templPathsStr="";
       for (int ind=0; ind < appPaths.size(); ++ind){
            templPathsStr += (appPaths.get(ind).asString() + ";").c_str();
       }
       config.put("templpath", templPathsStr.toLatin1().data());

    }

    if(!config.check("load_subfolders")){
        config.put("load_subfolders", "no");
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

#if defined(WIN32)
    //setup signal handler for windows
//    ACE_OS::signal(SIGINT, (ACE_SignalHandler) onSignal);
//    ACE_OS::signal(SIGBREAK, (ACE_SignalHandler) onSignal);
//    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) onSignal);

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
