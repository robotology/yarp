/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ymanager.h"

#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/application.h>
#include <dirent.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/ResourceFinder.h>

/*
 * TODO: using stringstream should be avoided to keep
 *  the compatibility with GUI
 */
#include <sstream>

#include <cstring>
#include <csignal>

using namespace yarp::os;
using namespace yarp::manager;

#if defined(_WIN32)
# define HEADER      ""
# define OKBLUE      ""
# define OKGREEN     ""
# define WARNING     ""
# define FAIL        ""
# define INFO        ""
# define ENDC        ""
#else
# include <unistd.h>
# include <cerrno>
# if defined(YARP_HAS_SYS_TYPES_H)
#  include <sys/types.h>
# endif
# if defined(YARP_HAS_SYS_WAIT_H)
#  include <sys/wait.h>
# endif
# if defined(YARP_HAS_SYS_PRCTL_H)
#  include <sys/prctl.h>
# endif
    std::string HEADER;
    std::string OKBLUE;
    std::string OKGREEN;
    std::string WARNING;
    std::string FAIL;
    std::string INFO;
    std::string ENDC;
#endif


#define CMD_COUNTS          24
#ifdef YARP_HAS_Libedit
    #include <editline/readline.h>
    const char* commands[CMD_COUNTS] = {"help", "exit","list mod", "list app", "list res", "add mod",
                  "add app", "add res", "load app", "run", "stop", "kill",
                  "connect", "disconnect", "which", "check state",
                  "check con", "check dep", "set", "get", "export",
                  "show mod", "assign hosts", " "};
    char* command_generator (const char* text, int state);
    char* appname_generator (const char* text, int state);
    char ** my_completion (const char* text, int start, int end);
    std::vector<std::string> appnames;
#endif

#define DEF_CONFIG_FILE     "ymanager.ini"

#define LOGO_MESSAGE "\
__   __\n\
\\ \\ / / __ ___   __ _ _ __   __ _  __ _  ___ _ __ \n\
 \\ V / '_ ` _ \\ / _` | '_ \\ / _` |/ _` |/ _ \\ '__|\n\
  | || | | | | | (_| | | | | (_| | (_| |  __/ |\n\
  |_||_| |_| |_|\\__,_|_| |_|\\__,_|\\__, |\\___|_|\n\
                                  |___/"

#define WELCOME_MESSAGE     "type \"help\" for more information."
#define VERSION_MESSAGE     "Version 1.1"

#define HELP_MESSAGE        "\
Usage:\n\
      yarpmanager-console [option...]\n\n\
Options:\n\
  --application <app>     Load a specific application identified by its xml file\n\
  --run                   Run the current application (should be used with --application)\n\
  --stop                  Stop the current application (should be used with --application)\n\
  --kill                  Kill the current application (should be used with --application)\n\
  --connect               Connect all connections from the current application (should be used with --application)\n\
  --disconnect            Disconnect all connections from the current application (should be used with --application)\n\
  --assign_hosts          Automatically assign modules to proper nodes using load balancer. (should be used with --application)\n\
  --check_dep             Check for all resource dependencies of the current application (should be used with --application)\n\
  --check_state           Check for running state of modules of the current application (should be used with --application) \n\
  --check_con             Check the connections states of the of the current application (should be used with --application)\n\
  --silent                Do not print the status messages (should be used with --application)\n\
  --exit                  Immediately exit after executing the commands (should be used with --application)\n\
  --from <conf>           Configuration file name\n\
  --version               Show current version\n\
  --help                  Show help\n"


#if defined(_WIN32)
static Manager* __pManager = NULL;
#endif

 bool isAbsolute(const char *path) {  //copied from yarp_OS ResourceFinder.cpp
        if (path[0]=='/'||path[0]=='\\') {
            return true;
        }
        std::string str(path);
        if (str.length()>1) {
            if (str[1]==':') {
                return true;
            }
        }
        return false;
    }

/**
 * Class YConsoleManager
 */

YConsoleManager::YConsoleManager(int argc, char* argv[]) : Manager()
{

#if defined(_WIN32)
    __pManager = (Manager*) this;
#endif

    bShouldRun = false;

    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("yarpmanager");
    rf.setDefaultConfigFile(DEF_CONFIG_FILE);
    rf.configure(argc, argv);

    yarp::os::Property config;
    config.fromString(rf.toString());

    if(config.check("help"))
    {
        std::cout<<HELP_MESSAGE<<'\n';
        return;
    }

    if(config.check("version"))
    {
        std::cout<<VERSION_MESSAGE<<'\n';
        return;
    }

    /**
     *  preparing default options
     */

    std::string inifile=rf.findFile("from");
    std::string inipath;
    size_t lastSlash=inifile.rfind('/');
    if (lastSlash != std::string::npos) {
        inipath=inifile.substr(0, lastSlash+1);
    } else {
        lastSlash=inifile.rfind('\\');
        if (lastSlash != std::string::npos) {
            inipath = inifile.substr(0, lastSlash + 1);
        }
    }

//     if(!config.check("ymanagerini_dir"))
//         config.put("ymanagerini_dir", inipath.c_str());

    if (!config.check("apppath")) {
        config.put("apppath", "./");
    }

    if (!config.check("modpath")) {
        config.put("modpath", "./");
    }

    if (!config.check("respath")) {
        config.put("respath", "./");
    }

    if (!config.check("load_subfolders")) {
        config.put("load_subfolders", "no");
    }

    if (!config.check("watchdog")) {
        config.put("watchdog", "no");
    }

    if (!config.check("module_failure")) {
        config.put("module_failure", "prompt");
    }

    if (!config.check("connection_failure")) {
        config.put("connection_failure", "prompt");
    }

    if (!config.check("auto_connect")) {
        config.put("auto_connect", "no");
    }

    if (!config.check("auto_dependency")) {
        config.put("auto_dependency", "no");
    }

    if (!config.check("color_theme")) {
        config.put("color_theme", "light");
    }


    /**
     * Set configuration
     */
    if (config.find("color_theme").asString() == "dark") {
        setColorTheme(THEME_DARK);
    } else if (config.find("color_theme").asString() == "light") {
        setColorTheme(THEME_LIGHT);
    } else {
        setColorTheme(THEME_NONE);
    }

    if (config.find("watchdog").asString() == "yes") {
        enableWatchDog();
    } else {
        disableWatchod();
    }

    if (config.find("auto_dependency").asString() == "yes") {
        enableAutoDependency();
    } else {
        disableAutoDependency();
    }

    if (config.find("auto_connect").asString() == "yes") {
        enableAutoConnect();
    } else {
        disableAutoConnect();
    }

    if(!config.check("silent"))
    {
        std::cout<<'\n'<<OKGREEN<<LOGO_MESSAGE<<ENDC<<'\n';
        std::cout<<'\n'<<WELCOME_MESSAGE<<'\n'<<'\n';
    }

    if(config.check("modpath"))
    {
        std::string strPath;
        std::stringstream modPaths(config.find("modpath").asString());
        while (getline(modPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str())) {
                strPath = std::string(inipath).append(strPath);
            }
            addModules(strPath.c_str());
        }
    }

    if(config.check("respath"))
    {
        std::string strPath;
        std::stringstream resPaths(config.find("respath").asString());
        while (getline(resPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str())) {
                strPath = std::string(inipath).append(strPath);
            }
            addResources(strPath.c_str());
        }
    }

    ErrorLogger* logger  = ErrorLogger::Instance();
    if(config.check("apppath"))
    {
        std::string strPath;
        std::stringstream appPaths(config.find("apppath").asString());
        while (getline(appPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str())) {
                strPath = std::string(inipath).append(strPath);
            }
            if(config.find("load_subfolders").asString() == "yes")
            {
                if (!loadRecursiveApplications(strPath.c_str())) {
                    logger->addError("Cannot load the applications from  " + strPath);
                }
            } else {
                addApplications(strPath.c_str());
            }
        }
    }

    reportErrors();

#ifdef YARP_HAS_Libedit
    updateAppNames(&appnames);
#endif


#if defined(_WIN32)
    ::signal(SIGINT, YConsoleManager::onSignal);
    ::signal(SIGBREAK, YConsoleManager::onSignal);
    ::signal(SIGTERM, YConsoleManager::onSignal);
#else
    struct sigaction new_action, old_action;

    /* Set up the structure to specify the new action. */
    new_action.sa_handler = YConsoleManager::onSignal;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction (SIGINT, nullptr, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGINT, &new_action, nullptr);
    }
    sigaction (SIGHUP, nullptr, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGHUP, &new_action, nullptr);
    }
    sigaction (SIGTERM, nullptr, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGTERM, &new_action, nullptr);
    }
#endif

    if(config.check("application"))
    {
        XmlAppLoader appload(config.find("application").asString().c_str());
        if(appload.init())
        {
            Application* application = appload.getNextApplication();
            if(application)
            {
                // add this application to the manager if does not exist
                if (!getKnowledgeBase()->getApplication(application->getName())) {
                    getKnowledgeBase()->addApplication(application);
                }

#ifdef YARP_HAS_Libedit
                updateAppNames(&appnames);
                #endif

                if(loadApplication(application->getName()))
                {
                    if (!config.check("silent")) {
                        which();
                    }

                    if (config.check("assign_hosts")) {
                        loadBalance();
                    }

                    if(config.check("run"))
                    {
                        if (config.check("connect")) {
                            enableAutoConnect();
                        }
                         bShouldRun = run();
                    } else if (config.check("connect")) {
                        connect();
                    }

                    if (config.check("disconnect")) {
                        disconnect();
                    }

                    if(config.check("stop"))
                    {
                        ExecutablePContainer modules = getExecutables();
                        ExecutablePIterator moditr;
                        unsigned int id = 0;
                        bShouldRun = false;
                        for(moditr=modules.begin(); moditr<modules.end(); moditr++)
                        {
                            if (running(id)) {
                                stop(id);
                            }
                            id++;
                        }
                        bShouldRun = !suspended();
                    }

                    if(config.check("kill"))
                    {
                         bShouldRun = false;
                         kill();
                    }

                    if (config.check("check_con")) {
                        checkConnections();
                    }

                    if (config.check("check_state")) {
                        checkStates();
                    }

                    if (config.check("check_dep")) {
                        if (checkDependency()) {
                            std::cout << INFO << "All of resource dependencies are satisfied." << ENDC << '\n';
                        }
                    }
                }
            }
        }
        if (!config.check("silent")) {
            reportErrors();
        }
    }

    if (!config.check("exit")) {
        YConsoleManager::myMain();
    }
}

YConsoleManager::~YConsoleManager() = default;



void YConsoleManager::onSignal(int signum)
{
#if defined(_WIN32)
    std::cout<<INFO<<"[force exit] yarpmanager will terminate all of the running modules on exit.";
    if( __pManager)
        __pManager->kill();
#else
    std::cout<<'\n'<<INFO<<"use"<<OKGREEN<<" 'exit' "<<INFO<<"to quit!"<<ENDC<<'\n';
#endif
}


void YConsoleManager::myMain()
{


#ifdef YARP_HAS_Libedit
    rl_attempted_completion_function = my_completion;
#endif

    while(!std::cin.eof())
    {
        std::string temp;

#ifdef YARP_HAS_Libedit
        static char* szLine = (char*)nullptr;
        if(szLine)
        {
            free(szLine);
            szLine = (char*)nullptr;
        }

        szLine = readline(">>");
        if(szLine && *szLine)
        {
            temp = szLine;
            add_history(szLine);
        } else {
            temp = "";
        }
#else
        std::cout << ">> ";
        getline(std::cin, temp);
#endif

        //Break string into separate strings on whitespace
        std::vector<std::string> cmdList;
        std::stringstream foo(temp);
        std::string s;
        while (foo >> s)
        {
            if (s[0] == '~') {
                s = getenv("HOME") + s.substr(1);
            }
            cmdList.push_back(s);
        }
        if(!process(cmdList))
        {
            if(cmdList[0] == "exit")
            {
                if (YConsoleManager::exit()) {
                    break;
                }
            }
            else
            {
                std::cout<<"'"<<cmdList[0]<<"'"<<INFO<<" is not correct. ";
                std::cout<<"type \"help\" for more information."<<ENDC<<'\n';
            }
        }
    }

#if defined(_WIN32)
    if(bShouldRun)
    {
        kill();
        reportErrors();
    }
#endif
    std::cout<<"bye."<<'\n';

}

bool YConsoleManager::exit()
{
    if (!bShouldRun) {
        return true;
    }

    std::string ans;
    std::cout<<WARNING<<"WARNING: ";
    std::cout<<INFO<<"You have some running modules. You might not be able to recover them later. Are you sure? [No/yes] "<<ENDC;
    getline(std::cin, ans);
    if(compareString(ans.c_str(),"yes"))
    {
        bShouldRun = false;
        //kill();
        //reportErrors();
        return true;
    }
    return false;
}


bool YConsoleManager::process(const std::vector<std::string> &cmdList)
{
    if (!cmdList.size() || cmdList[0] == "") {
        return true;
    }

    /**
     * help
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "help"))
     {
         help();
         return true;
     }


    /**
     * add application
     */
     if((cmdList.size() == 3) &&
        (cmdList[0] == "add") && (cmdList[1] == "app"))
     {
         if (addApplication(cmdList[2].c_str())) {
             std::cout << INFO << cmdList[2] << " is successfully added." << ENDC << '\n';
         }
         reportErrors();
        #ifdef YARP_HAS_Libedit
        updateAppNames(&appnames);
        #endif
         return true;
     }

    /**
     * add module
     */
     if((cmdList.size() == 3) &&
        (cmdList[0] == "add") && (cmdList[1] == "mod"))
     {
         if (addModule(cmdList[2].c_str())) {
             std::cout << INFO << cmdList[2] << " is successfully added." << ENDC << '\n';
         }
         reportErrors();
         return true;
     }

    /**
     * add resource
     */
     if((cmdList.size() == 3) &&
        (cmdList[0] == "add") && (cmdList[1] == "res"))
     {
         if (addResource(cmdList[2].c_str())) {
             std::cout << INFO << cmdList[2] << " is successfully added." << ENDC << '\n';
         }
         reportErrors();
         return true;
     }


    /**
     * load application
     */
     if((cmdList.size() == 3) &&
        (cmdList[0] == "load") && (cmdList[1] == "app"))
     {
         if(loadApplication(cmdList[2].c_str()))
         {
            //std::cout<<cmdList[2]<<" is successfully loaded."<<'\n';
            which();
         }
         reportErrors();
         return true;
     }

    /**
     * load module
     */
    /*
     if((cmdList.size() >= 3) &&
        (cmdList[0] == "load") && (cmdList[1] == "mod"))
     {
         if(cmdList.size() > 3)
         {
            if(manager.loadModule(cmdList[2].c_str(), cmdList[3].c_str()))
                std::cout<<cmdList[2]<<" is successfully loaded."<<'\n';
         }
        else
            if(manager.loadModule(cmdList[2].c_str()))
                std::cout<<cmdList[2]<<" is successfully loaded."<<'\n';
         reportErrors();
         return true;
     }
    */

    /**
     * run
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "run"))
     {
         bShouldRun = run();
         reportErrors();
         return true;
     }
     if((cmdList.size() >= 2) &&
        (cmdList[0] == "run"))
     {
        bShouldRun = false;
        for (unsigned int i = 1; i < cmdList.size(); i++) {
            bShouldRun |= run(atoi(cmdList[i].c_str()));
        }
        reportErrors();
        return true;
     }

    /**
     * stop
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "stop"))
     {
         bShouldRun = false;
         stop();
         reportErrors();
         return true;
     }
     if((cmdList.size() >= 2) &&
        (cmdList[0] == "stop"))
     {
         //bShouldRun = false;
         for (unsigned int i = 1; i < cmdList.size(); i++) {
             stop(atoi(cmdList[i].c_str()));
         }
         bShouldRun = !suspended();
         reportErrors();
         return true;
     }

    /**
     * kill
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "kill"))
     {
         bShouldRun = false;
         kill();
         reportErrors();
         return true;
     }
     if((cmdList.size() >= 2) &&
        (cmdList[0] == "kill"))
     {
         //bShouldRun = false;
         for (unsigned int i = 1; i < cmdList.size(); i++) {
             kill(atoi(cmdList[i].c_str()));
         }
         bShouldRun = !suspended();
         reportErrors();
         return true;
     }

    /**
     * connect
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "connect"))
     {
         connect();
         reportErrors();
         return true;
     }
     if((cmdList.size() >= 2) &&
        (cmdList[0] == "connect"))
     {
         for (unsigned int i = 1; i < cmdList.size(); i++) {
             connect(atoi(cmdList[i].c_str()));
         }
        reportErrors();
        return true;
     }


    /**
     * disconnect
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "disconnect"))
     {
         disconnect();
         reportErrors();
         return true;
     }
     if((cmdList.size() >= 2) &&
        (cmdList[0] == "disconnect"))
     {
         for (unsigned int i = 1; i < cmdList.size(); i++) {
             disconnect(atoi(cmdList[i].c_str()));
         }
        reportErrors();
        return true;
     }


    /**
     * which
     */
     if((cmdList.size() == 1) &&
        (cmdList[0] == "which"))
     {
        which();
        return true;
     }

    /**
     * check for dependencies
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "check") && (cmdList[1] == "dep"))
    {
        if (checkDependency()) {
            std::cout << INFO << "All of resource dependencies are satisfied." << ENDC << '\n';
        }
        reportErrors();
        return true;
    }


    /**
     * check for running state
     */
    if((cmdList.size() == 3) &&
        (cmdList[0] == "check") && (cmdList[1] == "state"))
    {
        ExecutablePContainer modules = getExecutables();
        auto id = (unsigned int)atoi(cmdList[2].c_str());
        if(id>=modules.size())
        {
            std::cout<<FAIL<<"ERROR:   "<<INFO<<"Module id is out of range."<<ENDC<<'\n';
            return true;
        }

        if (running(id)) {
            std::cout<<OKGREEN<<"<RUNNING> ";
        } else {
            std::cout << FAIL << "<STOPPED> ";
        }
        std::cout<<INFO<<"("<<id<<") ";
        std::cout<<modules[id]->getCommand();
        std::cout<<" ["<<modules[id]->getHost()<<"]"<<ENDC<<'\n';
        reportErrors();
        return true;
    }
    if((cmdList.size() == 2) &&
        (cmdList[0] == "check") && (cmdList[1] == "state"))
    {
        checkStates();
        reportErrors();
        return true;
    }


    /**
     * check for connection state
     */
    if((cmdList.size() == 3) &&
        (cmdList[0] == "check") && (cmdList[1] == "con"))
    {

        CnnContainer connections  = getConnections();
        auto id = (unsigned int)atoi(cmdList[2].c_str());
        if(id>=connections.size())
        {
            std::cout<<FAIL<<"ERROR:   "<<INFO<<"Connection id is out of range."<<ENDC<<'\n';
            return true;
        }

        if (connected(id)) {
            std::cout<<OKGREEN<<"<CONNECTED> ";
        } else {
            std::cout << FAIL << "<DISCONNECTED> ";
        }

        std::cout<<INFO<<"("<<id<<") ";
        std::cout<<connections[id].from()<<" - "<<connections[id].to();
                std::cout<<" ["<<connections[id].carrier()<<"]"<<ENDC<<'\n';
        reportErrors();
        return true;
    }
    if((cmdList.size() == 2) &&
        (cmdList[0] == "check") && (cmdList[1] == "con"))
    {
        checkConnections();
        return true;
    }

    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};

    /**
     *  list available modules
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "list") && (cmdList[1] == "mod"))
    {
        KnowledgeBase* kb = getKnowledgeBase();
        ModulePContainer mods =  kb->getModules();
        int id = 0;
        for(auto& mod : mods)
        {
            std::string fname;
            std::string fpath = mod->getXmlFile();

            size_t pos = fpath.rfind(directorySeparator);
            if (pos != std::string::npos) {
                fname = fpath.substr(pos);
            } else {
                fname = fpath;
            }
            std::cout<<INFO<<"("<<id++<<") ";
            std::cout<<OKBLUE<<mod->getName()<<ENDC;
            std::cout<<INFO<<" ["<<fname<<"]"<<ENDC<<'\n';
        }
        return true;
    }


    /**
     *  list available applications
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "list") && (cmdList[1] == "app"))
    {
        KnowledgeBase* kb = getKnowledgeBase();
        ApplicaitonPContainer apps =  kb->getApplications();
        int id = 0;
        for(auto& app : apps)
        {
            std::string fname;
            std::string fpath = app->getXmlFile();

            size_t pos = fpath.rfind(directorySeparator);
            if (pos != std::string::npos) {
                fname = fpath.substr(pos);
            } else {
                fname = fpath;
            }
            std::cout<<INFO<<"("<<id++<<") ";
            std::cout<<OKBLUE<<app->getName()<<ENDC;
            std::cout<<INFO<<" ["<<fname<<"]"<<ENDC<<'\n';
        }
        return true;
    }

    /**
     *  list available resources
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "list") && (cmdList[1] == "res"))
    {
        KnowledgeBase* kb = getKnowledgeBase();
        ResourcePContainer resources = kb->getResources();
        int id = 0;
        for(auto& resource : resources)
        {
            auto* comp = dynamic_cast<Computer*>(resource);
            if(comp)
            {
                std::string fname;
                std::string fpath = comp->getXmlFile();
                size_t pos = fpath.rfind(directorySeparator);
                if (pos != std::string::npos) {
                    fname = fpath.substr(pos);
                } else {
                    fname = fpath;
                }
                std::cout<<INFO<<"("<<id++<<") ";
                if (comp->getDisable()) {
                    std::cout<<WARNING<<comp->getName()<<ENDC;
                } else {
                    std::cout << OKBLUE << comp->getName() << ENDC;
                }
                std::cout<<INFO<<" ["<<fname<<"]"<<ENDC<<'\n';
            }
        }
        return true;
    }


    /**
     *  export knowledgebase graph
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "export") )
    {
        if (!exportDependencyGraph(cmdList[1].c_str())) {
            std::cout << FAIL << "ERROR:   " << INFO << "Cannot export graph to " << cmdList[1] << "." << ENDC << '\n';
        }
        return true;
    }


    /**
     * show module's information
     */
    if((cmdList.size() == 3) &&
        (cmdList[0] == "show") && (cmdList[1] == "mod"))
    {
        KnowledgeBase* kb = getKnowledgeBase();
        if(!kb->getModule(cmdList[2].c_str()))
        {
            std::cout<<FAIL<<"ERROR:   "<<INFO<<"'"<<cmdList[2].c_str()<<"' not found."<<ENDC<<'\n';
            return true;
        }
        std::cout<<INFO;
        PRINT_MODULE(kb->getModule(cmdList[2].c_str()));
        std::cout<<ENDC;
        return true;
    }

    /**
     * set an option
     */
     if((cmdList.size() == 3) &&
        (cmdList[0] == "set"))
     {
         config.unput(cmdList[1]);
         config.put(cmdList[1], cmdList[2]);

        if(cmdList[1] == std::string("watchdog"))
        {
            if (cmdList[2] == std::string("yes")) {
                enableWatchDog();
            } else {
                disableWatchod();
            }
        }

        if(cmdList[1] == std::string("auto_dependency"))
        {
            if (cmdList[2] == std::string("yes")) {
                enableAutoDependency();
            } else {
                disableAutoDependency();
            }
        }

        if(cmdList[1] == std::string("auto_connect"))
        {
            if (cmdList[2] == std::string("yes")) {
                enableAutoConnect();
            } else {
                disableAutoConnect();
            }
        }

        if(cmdList[1] == std::string("color_theme"))
        {
            if (cmdList[2] == std::string("dark")) {
                setColorTheme(THEME_DARK);
            } else if (cmdList[2] == std::string("light")) {
                setColorTheme(THEME_LIGHT);
            } else {
                setColorTheme(THEME_NONE);
            }
        }

        return true;
     }


    /**
     * get an option
     */
     if((cmdList.size() == 2) &&
        (cmdList[0] == "get"))
     {
         if(config.check(cmdList[1]))
         {
            std::cout<<OKBLUE<<cmdList[1]<<INFO<<" = ";
            std::cout<<OKGREEN<<config.find(cmdList[1]).asString()<<ENDC<<'\n';
         } else {
             std::cout << FAIL << "ERROR:   " << INFO << "'" << cmdList[1].c_str() << "' not found." << ENDC << '\n';
         }
         return true;
     }

    /**
     * load balancing
     */
    if((cmdList.size() == 2) &&
        (cmdList[0] == "assign") && (cmdList[1] == "hosts"))
    {
        loadBalance();
        reportErrors();
        return true;
    }

    return false;
}


void YConsoleManager::help()
{
    std::cout<<"Here is a list of YARP manager keywords.\n"<<'\n';
    std::cout<<OKGREEN<<"help"<<INFO<<"                    : show help."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"exit"<<INFO<<"                    : exit yarp manager."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"list mod"<<INFO<<"                : list available modules."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"list app"<<INFO<<"                : list available applications."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"list res"<<INFO<<"                : list available resources. (i.e. nodes in a cluster)"<<ENDC<<'\n';
    std::cout<<OKGREEN<<"add mod <filename>"<<INFO<<"      : add a module from its description file."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"add app <filename>"<<INFO<<"      : add an application from its description file."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"add res <filename>"<<INFO<<"      : add resources from a description file."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"load app <application>"<<INFO<<"  : load an application to run."<<'\n';
//  std::cout<<"load mod <module> <host>: load a module to run on an optional host."<<'\n';
    std::cout<<OKGREEN<<"run [IDs]"<<INFO<<"               : run application or a modules indicated by IDs."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"stop [IDs]"<<INFO<<"              : stop running application or modules indicated by IDs."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"kill [IDs]"<<INFO<<"              : kill running application or modules indicated by IDs."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"connect [IDs]"<<INFO<<"           : stablish all connections or just one connection indicated by IDs."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"disconnect [IDs]"<<INFO<<"        : remove all connections or just one connection indicated by IDs."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"which"<<INFO<<"                   : list loaded modules, connections and resource dependencies."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"check dep"<<INFO<<"               : check for all resource dependencies."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"check state [id]"<<INFO<<"        : check for running state of application or a module indicated by id."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"check con [id]"<<INFO<<"          : check for all connections state or just one connection indicated by id."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"set <option> <value>"<<INFO<<"    : set value to an option."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"get <option>"<<INFO<<"            : show value of an option."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"export <filename>"<<INFO<<"       : export application's graph as Graphviz dot format."<<ENDC<<'\n';
//  std::cout<<"edit mod <modname>      : open module relevant xml file to edit."<<'\n';
//  std::cout<<"edit app <appname>      : open application relevant xml file to edit."<<'\n';
    std::cout<<OKGREEN<<"show mod <modname>"<<INFO<<"      : display module information (description, input, output,...)."<<ENDC<<'\n';
    std::cout<<OKGREEN<<"assign hosts"<<INFO<<"            : automatically assign modules to proper nodes using load balancer."<<ENDC<<'\n';

    std::cout<<'\n';
}


void YConsoleManager::which()
{
    ExecutablePContainer modules = getExecutables();
    CnnContainer connections  = getConnections();
    ExecutablePIterator moditr;
    CnnIterator cnnitr;

    std::cout<<'\n'<<HEADER<<"Application: "<<ENDC<<'\n';
    std::cout<<OKBLUE<<getApplicationName()<<ENDC<<'\n';

    std::cout<<'\n'<<HEADER<<"Modules: "<<ENDC<<'\n';
    int id = 0;
    for(moditr=modules.begin(); moditr<modules.end(); moditr++)
    {
        std::cout<<INFO<<"("<<id++<<") ";
        std::cout<<OKBLUE<<(*moditr)->getCommand()<<INFO;
        std::cout<<" ["<<(*moditr)->getHost()<<"] ["<<(*moditr)->getParam()<<"]";
        std::cout<<" ["<<(*moditr)->getEnv()<<"]"<<ENDC<<'\n';
    }
    std::cout<<'\n'<<HEADER<<"Connections: "<<ENDC<<'\n';
    id = 0;
    for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
    {
        std::cout<<INFO<<"("<<id++<<") ";
        std::cout<<OKBLUE<<(*cnnitr).from()<<" - "<<(*cnnitr).to()<<INFO;
            std::cout<<" ["<<(*cnnitr).carrier()<<"]";
        std::cout<<ENDC<<'\n';
    }

    std::cout<<'\n'<<HEADER<<"Resources:"<<ENDC<<'\n';
    id = 0;
    ResourcePIterator itrS;
    for(itrS=getResources().begin(); itrS!=getResources().end(); itrS++)
    {
        std::cout<<INFO<<"("<<id++<<") ";
        std::cout<<OKBLUE<<(*itrS)->getName()<<INFO<<" ["<<(*itrS)->getTypeName()<<"]"<<ENDC<<'\n';
    }
    std::cout<<'\n';
}

void YConsoleManager::checkStates()
{
    ExecutablePContainer modules = getExecutables();
    ExecutablePIterator moditr;
    unsigned int id = 0;
    bShouldRun = false;
    for(moditr=modules.begin(); moditr<modules.end(); moditr++)
    {
        if(running(id))
        {
            bShouldRun = true;
            std::cout<<OKGREEN<<"<RUNNING> ";
        } else {
            std::cout << FAIL << "<STOPPED> ";
        }
        std::cout<<INFO<<"("<<id<<") ";
        std::cout<<(*moditr)->getCommand();
        std::cout<<" ["<<(*moditr)->getHost()<<"]"<<ENDC<<'\n';
        id++;
    }
}

void YConsoleManager::checkConnections()
{
    CnnContainer connections  = getConnections();
    CnnIterator cnnitr;
    int id = 0;
    for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
    {
        if (connected(id)) {
            std::cout<<OKGREEN<<"<CONNECTED> ";
        } else {
            std::cout << FAIL << "<DISCONNECTED> ";
        }

        std::cout<<INFO<<"("<<id<<") ";
        std::cout<<(*cnnitr).from()<<" - "<<(*cnnitr).to();
            std::cout<<" ["<<(*cnnitr).carrier()<<"]"<<ENDC<<'\n';
        id++;
    }
}

void YConsoleManager::reportErrors()
{
    ErrorLogger* logger  = ErrorLogger::Instance();
    if(logger->errorCount() || logger->warningCount())
    {
        const char* msg;
        while ((msg = logger->getLastError())) {
            std::cout << FAIL << "ERROR:   " << INFO << msg << ENDC << '\n';
        }

        while ((msg = logger->getLastWarning())) {
            std::cout << WARNING << "WARNING: " << INFO << msg << ENDC << '\n';
        }
    }
}


void YConsoleManager::onExecutableStart(void* which) { }

void YConsoleManager::onExecutableStop(void* which) { }

void YConsoleManager::onExecutableDied(void* which) { }

void YConsoleManager::onExecutableFailed(void* which)
{
    auto* exe = (Executable*) which;
    if (config.find("module_failure").asString() == "prompt") {
        std::cout << exe->getCommand() << " from " << exe->getHost() << " is failed!" << '\n';
    }

    if(config.find("module_failure").asString() == "recover")
    {
        std::cout<<'\n'<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! (restarting...)"<<'\n';
        exe->start();
    }

    if(config.find("module_failure").asString() == "terminate")
    {
        std::cout<<'\n'<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! (terminating application...)"<<'\n';
        bShouldRun = false;
        stop();
        reportErrors();
    }
}

void YConsoleManager::onCnnStablished(void* which) { }

void YConsoleManager::onCnnFailed(void* which)
{
    auto* cnn = (Connection*) which;
    if (config.check("connection_failure") && config.find("connection_failure").asString() == "prompt") {
        std::cout << '\n'
             << "connection failed between " << cnn->from() << " and " << cnn->to() << '\n';
    }

    if(bShouldRun && config.check("connection_failure") &&
     config.find("connection_failure").asString() == "terminate")
     {
        std::cout<<'\n'<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<"(terminating application...)"<<'\n';
        bShouldRun = false;
        stop();
        reportErrors();
     }
}


bool YConsoleManager::loadRecursiveApplications(const char* szPath)
{
    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
    std::string strPath = szPath;
    if ((strPath.rfind(directorySeparator) == std::string::npos) || (strPath.rfind(directorySeparator) != strPath.size() - 1)) {
        strPath = strPath + std::string(directorySeparator);
    }

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == nullptr) {
        return false;
    }

    addApplications(strPath.c_str());

    while((entry = readdir(dir)))
    {
        if((std::string(entry->d_name) != std::string("."))
        && (std::string(entry->d_name) != std::string("..")))
        {
            std::string name = strPath + std::string(entry->d_name);
            loadRecursiveApplications(name.c_str());
        }
    }
    closedir(dir);
    return true;
}



void YConsoleManager::updateAppNames(std::vector<std::string>* names)
{
    names->clear();
    KnowledgeBase* kb = getKnowledgeBase();
    ApplicaitonPContainer apps =  kb->getApplications();
    for (auto& app : apps) {
        names->push_back(app->getName());
    }
}



void YConsoleManager::setColorTheme(ColorTheme theme)
{

#if defined(_WIN32)
    // do nothing here
#else
    switch(theme) {
        case THEME_DARK : {
            HEADER = "\033[01;95m";
            OKBLUE = "\033[94m";
            OKGREEN = "\033[92m";
            WARNING = "\033[93m";
            FAIL = "\033[91m";
            INFO = "\033[37m";
            ENDC = "\033[0m";
            break;
        }
        case THEME_LIGHT: {
            HEADER = "\033[01;35m";
            OKBLUE = "\033[34m";
            OKGREEN = "\033[32m";
            WARNING = "\033[33m";
            FAIL = "\033[31m";
            INFO = "\033[0m";
            ENDC = "\033[0m";
            break;
        }
        default: {
            HEADER = "";
            OKBLUE = "";
            OKGREEN = "";
            WARNING = "";
            FAIL = "";
            INFO = "";
            ENDC = "";
            break;
        }
    };
#endif

}



#ifdef YARP_HAS_Libedit

char* dupstr(char* s)
{
  char *r;
  r = (char*) malloc ((strlen (s) + 1));
  strcpy (r, s);
  return (r);
}

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or NULL if there aren't any. */
char ** my_completion (const char* text, int start, int end)
{
    char **matches;

    matches = (char **)nullptr;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
    if (start == 0) {
        matches = rl_completion_matches(text, &command_generator);
    } else {
        matches = rl_completion_matches(text, &appname_generator);
    }

    return (matches);
}


/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char* command_generator (const char* text, int state)
{
  //printf("commmand_genrator\n");

  static int list_index, len;
  char *name;

  /* if this is a new word to complete, initialize now.  this includes
     saving the length of text for efficiency, and initializing the index
     variable to 0. */
  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  while ((list_index<CMD_COUNTS) && (name = (char*)commands[list_index]))
    {
      list_index++;
      if (strncmp(name, text, len) == 0) {
          return (dupstr(name));
      }
    }

  /* if no names matched, then return null. */
  return ((char *)nullptr);
}


char* appname_generator (const char* text, int state)
{

  static unsigned int list_index, len;
  char *name;

   if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  while ((list_index<appnames.size()) && (name = (char*)appnames[list_index].c_str()))
    {
      list_index++;
      if (strncmp(name, text, len) == 0) {
          return (dupstr(name));
      }
    }

  return ((char *)nullptr);
}

#endif
