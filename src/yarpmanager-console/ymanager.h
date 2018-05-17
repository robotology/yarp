/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Istituto Italiano di Tecnologia (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 *
 */


#ifndef YCONSOLEMANAGER
#define YCONSOLEMANAGER


#include <yarp/manager/manager.h>
#include <yarp/manager/utility.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <string>

//namespace ymm {

typedef enum _ColorTheme {
    THEME_DARK,
    THEME_LIGHT,
    THEME_NONE
} ColorTheme;


/**
 * Class YConsoleManager
 */
class YConsoleManager : public yarp::manager::Manager {

public:
    YConsoleManager(int argc, char* argv[]);
    ~YConsoleManager();


protected:
    virtual void onExecutableStart(void* which) override;
    virtual void onExecutableStop(void* which) override;
    virtual void onExecutableDied(void* which) override;
    virtual void onExecutableFailed(void* which) override;
    virtual void onCnnStablished(void* which) override;
    virtual void onCnnFailed(void* which) override;

private:
    bool bShouldRun;
    yarp::os::Property cmdline;
    yarp::os::Property config;

    void help(void);
    void myMain(void);
    bool process(const std::vector<std::string> &cmdList);
    bool exit(void);
    void reportErrors(void);
    void which(void);
    void checkStates(void);
    void checkConnections(void);
    bool loadRecursiveApplications(const char* szPath);
    void updateAppNames(std::vector<std::string>* apps);
    void setColorTheme(ColorTheme theme);
    static void onSignal(int signum);
};


#endif //YCONSOLEMANAGER
