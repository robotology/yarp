/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
    ~YConsoleManager() override;


protected:
    void onExecutableStart(void* which) override;
    void onExecutableStop(void* which) override;
    void onExecutableDied(void* which) override;
    void onExecutableFailed(void* which) override;
    void onCnnStablished(void* which) override;
    void onCnnFailed(void* which) override;

private:
    bool bShouldRun;
    yarp::os::Property cmdline;
    yarp::os::Property config;

    void help();
    void myMain();
    bool process(const std::vector<std::string> &cmdList);
    bool exit();
    void reportErrors();
    void which();
    void checkStates();
    void checkConnections();
    bool loadRecursiveApplications(const char* szPath);
    void updateAppNames(std::vector<std::string>* apps);
    void setColorTheme(ColorTheme theme);
    static void onSignal(int signum);
};


#endif //YCONSOLEMANAGER
