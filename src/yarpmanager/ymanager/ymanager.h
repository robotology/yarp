// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef __YCONSOLEMANAGER__
#define __YCONSOLEMANAGER__


#include "manager.h"
#include "utility.h"
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

using namespace std; 

//namespace ymm {

typedef enum _ColorTheme {
    THEME_DARK,
    THEME_LIGHT,
    THEME_NONE
} ColorTheme;


/**
 * Class YConsoleManager  
 */
class YConsoleManager : public Manager {

public: 
    YConsoleManager(int argc, char* argv[]);
    ~YConsoleManager();


protected:
    virtual void onExecutableStart(void* which);
    virtual void onExecutableStop(void* which);
    virtual void onExecutableDied(void* which);
    virtual void onExecutableFailed(void* which);
    virtual void onCnnStablished(void* which);
    virtual void onCnnFailed(void* which);

private:
    bool bShouldRun;
    yarp::os::Property cmdline;
    yarp::os::Property config;
    
    void help(void);
    void myMain(void);
    bool process(const vector<string> &cmdList);
    bool exit(void);
    void reportErrors(void);
    void which(void);
    void checkStates(void);
    void checkConnections(void);
    bool loadRecursiveApplications(const char* szPath);
    void updateAppNames(vector<string>* apps);
    void setColorTheme(ColorTheme theme);
    static void onSignal(int signum);
};


#endif //__YCONSOLEMANAGER__
