/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <string.h>
#include "ymanager.h"
#include "xmlapploader.h"
#include "application.h"
#include "ymm-dir.h"

/*
 * TODO: using stringstream should be avoided to keep 
 *  the compatibility with GUI
 */
#include <sstream>


#include <yarp/os/ResourceFinder.h>

using namespace yarp::os;


#if defined(WIN32)
    #include <yarp/os/impl/PlatformSignal.h>
    #define HEADER      ""
    #define OKBLUE      ""
    #define OKGREEN     ""
    #define WARNING     ""
    #define FAIL        ""
    #define INFO        ""
    #define ENDC        ""
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <signal.h>

    string HEADER = "";
    string OKBLUE = "";
    string OKGREEN = "";
    string WARNING = "";
    string FAIL = "";
    string INFO = "";
    string ENDC = "";
#endif


#define CMD_COUNTS          24
#ifdef WITH_READLINE
    #include <readline/readline.h>
    #include <readline/history.h>
    const char* commands[CMD_COUNTS] = {"help", "exit","list mod", "list app", "list res", "add mod",
                  "add app", "add res", "load app", "run", "stop", "kill", 
                  "connect", "disconnect", "which", "check state",
                  "check con", "check dep", "set", "get", "export",
                  "show mod", "assign hosts", " "};
    char* command_generator (const char* text, int state);
    char* appname_generator (const char* text, int state);
    char ** my_completion (const char* text, int start, int end);
    vector<string> appnames;    
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
      ymanager [option...]\n\n\
Options:\n\
  --help                  Show help\n\
  --from                  Configuration file name\n\
  --version               Show current version\n"


#if defined(WIN32)
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

#if defined(WIN32)
    __pManager = (Manager*) this;
#endif

    bShouldRun = false;

    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose(false);
    rf.setDefaultContext("");
    rf.setDefaultConfigFile(DEF_CONFIG_FILE);
    rf.configure(argc, argv);

    yarp::os::Property config;
    config.fromString(rf.toString());

    if(config.check("help"))
    {
        cout<<HELP_MESSAGE<<endl;
        return;
    }
 
    if(config.check("version"))
    {
        cout<<VERSION_MESSAGE<<endl;
        return;
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
    
//     if(!config.check("ymanagerini_dir"))   
//         config.put("ymanagerini_dir", inipath.c_str());
    
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

    if(!config.check("color_theme"))
        config.put("color_theme", "light");

    
    /**
     * Set configuration
     */
    if(config.find("color_theme").asString() == "dark")
        setColorTheme(THEME_DARK);
    else if(config.find("color_theme").asString() == "light")
        setColorTheme(THEME_LIGHT);
    else
        setColorTheme(THEME_NONE);

    if(config.find("watchdog").asString() == "yes")
        enableWatchDog();
    else
        disableWatchod();

    if(config.find("auto_dependency").asString() == "yes")
        enableAutoDependency();
    else
        disableAutoDependency();

    if(config.find("auto_connect").asString() == "yes")
        enableAutoConnect();
    else
        disableAutoConnect();

    cout<<endl<<OKGREEN<<LOGO_MESSAGE<<ENDC<<endl;
    cout<<endl<<WELCOME_MESSAGE<<endl<<endl;

    if(config.check("modpath"))
    {
        string strPath;
        stringstream modPaths(config.find("modpath").asString().c_str());
        while (getline(modPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=inipath+strPath;
            addModules(strPath.c_str());
        }
    }

    if(config.check("respath"))
    {
        string strPath;
        stringstream resPaths(config.find("respath").asString().c_str());
        while (getline(resPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=inipath+strPath;
            addResources(strPath.c_str());
        }
    }

    ErrorLogger* logger  = ErrorLogger::Instance(); 
    if(config.check("apppath"))
    {
        string strPath;
        stringstream appPaths(config.find("apppath").asString().c_str());
        while (getline(appPaths, strPath, ';'))
        {
            trimString(strPath);
            if (!isAbsolute(strPath.c_str()))
                strPath=inipath+strPath;
            if(config.find("load_subfolders").asString() == "yes")
            {
                if(!loadRecursiveApplications(strPath.c_str()))
                    logger->addError("Cannot load the applications from  " + strPath);
            }        
            else
                addApplications(strPath.c_str()); 
        }
    }

    reportErrors(); 

#ifdef WITH_READLINE
    updateAppNames(&appnames);
#endif


#if defined(WIN32)
    ACE_OS::signal(SIGINT, (ACE_SignalHandler) YConsoleManager::onSignal);
    ACE_OS::signal(SIGBREAK, (ACE_SignalHandler) YConsoleManager::onSignal);
    ACE_OS::signal(SIGTERM, (ACE_SignalHandler) YConsoleManager::onSignal);
#else
    struct sigaction new_action, old_action;
     
    /* Set up the structure to specify the new action. */
    new_action.sa_handler = YConsoleManager::onSignal;
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

    if(config.check("application"))
    {
        XmlAppLoader appload(config.find("application").asString().c_str());
        if(appload.init())
        {
            Application* application = appload.getNextApplication();
            if(application)
            {
                // add this application to the manager if does not exist    
                if(!getKnowledgeBase()->getApplication(application->getName()))
                    getKnowledgeBase()->addApplication(application);

                #ifdef WITH_READLINE
                updateAppNames(&appnames);
                #endif

                if(loadApplication(application->getName()))
                {
                    which();
                    if(config.check("run"))
                    {
                         bShouldRun = run();
                         reportErrors();
                    }
                }
                    
            }

        }
        reportErrors();
    }

    YConsoleManager::myMain();  
}

YConsoleManager::~YConsoleManager()
{
}



void YConsoleManager::onSignal(int signum)
{
#if defined(WIN32)
    cout<<INFO<<"[force exit] yarpmanager will terminate all of the running modules on exit.";
    if( __pManager)
        __pManager->kill();
#else
    cout<<endl<<INFO<<"use"<<OKGREEN<<" 'exit' "<<INFO<<"to quit!"<<ENDC<<endl;
#endif
}


void YConsoleManager::myMain(void)
{

        
#ifdef WITH_READLINE
    rl_attempted_completion_function = my_completion;
#endif

    while(!cin.eof())
    {
        string temp;

#ifdef WITH_READLINE
        static char* szLine = (char*)NULL;
        if(szLine)
        {
            free(szLine);
            szLine = (char*)NULL;
        }
            
        szLine = readline(">>");
        if(szLine && *szLine)
        {
            temp = szLine;
            add_history(szLine);
        }
        else 
            temp = "";  
#else
        cout << ">> ";
        getline(cin, temp);
#endif

        //Break string into separate strings on whitespace
        vector<string> cmdList;
        stringstream foo(temp);
        string s;
        while (foo >> s)
        {
            if (s[0]=='~') s = getenv("HOME") + s.substr(1);
            cmdList.push_back(s);
        }
        if(!process(cmdList))
        {
            if(cmdList[0] == "exit")
            {
                if(YConsoleManager::exit())
                    break;
            }
            else
            {
                cout<<"'"<<cmdList[0]<<"'"<<INFO<<" is not correct. ";
                cout<<"type \"help\" for more information."<<ENDC<<endl;
            }
        }
    }

#if defined(WIN32)
    if(bShouldRun)
    {
        kill();
        reportErrors();
    }
#endif
    cout<<"bye."<<endl;

}

bool YConsoleManager::exit(void)
{
    if(!bShouldRun)
        return true;

    string ans;
    cout<<WARNING<<"WARNING: ";
    cout<<INFO<<"You have some running modules. You might not be able to recover them later. Are you sure? [No/yes] "<<ENDC;
    getline(cin, ans);
    if(compareString(ans.c_str(),"yes"))
    {
        bShouldRun = false; 
        //kill();
        //reportErrors();
        return true;
    }
    return false;
}


bool YConsoleManager::process(const vector<string> &cmdList)
{
    if (!cmdList.size() || cmdList[0] == "") 
        return true;
    
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
         if(addApplication(cmdList[2].c_str()))
            cout<<INFO<<cmdList[2]<<" is successfully added."<<ENDC<<endl;
         reportErrors();
        #ifdef WITH_READLINE
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
         if(addModule(cmdList[2].c_str()))
            cout<<INFO<<cmdList[2]<<" is successfully added."<<ENDC<<endl;
         reportErrors();
         return true;
     }

    /**
     * add resource
     */ 
     if((cmdList.size() == 3) && 
        (cmdList[0] == "add") && (cmdList[1] == "res"))
     {
         if(addResource(cmdList[2].c_str()))
            cout<<INFO<<cmdList[2]<<" is successfully added."<<ENDC<<endl;
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
            //cout<<cmdList[2]<<" is successfully loaded."<<endl;
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
                cout<<cmdList[2]<<" is successfully loaded."<<endl;
         }
        else
            if(manager.loadModule(cmdList[2].c_str()))
                cout<<cmdList[2]<<" is successfully loaded."<<endl;     
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
        for(unsigned int i=1; i<cmdList.size(); i++)
            bShouldRun |= run(atoi(cmdList[i].c_str()));
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
        for(unsigned int i=1; i<cmdList.size(); i++) 
            stop(atoi(cmdList[i].c_str()));
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
         for(unsigned int i=1; i<cmdList.size(); i++) 
            kill(atoi(cmdList[i].c_str()));
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
        for(unsigned int i=1; i<cmdList.size(); i++) 
            connect(atoi(cmdList[i].c_str()));
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
        for(unsigned int i=1; i<cmdList.size(); i++) 
            disconnect(atoi(cmdList[i].c_str()));
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
        if(checkDependency())
            cout<<INFO<<"All of resource dependencies are satisfied."<<ENDC<<endl;
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
        unsigned int id = (unsigned int)atoi(cmdList[2].c_str());
        if(id>=modules.size())
        {
            cout<<FAIL<<"ERROR:   "<<INFO<<"Module id is out of range."<<ENDC<<endl;
            return true;
        }

        if(running(id))
            cout<<OKGREEN<<"<RUNNING> ";
        else
            cout<<FAIL<<"<STOPPED> ";
        cout<<INFO<<"("<<id<<") ";
        cout<<modules[id]->getCommand();
        cout<<" ["<<modules[id]->getHost()<<"]"<<ENDC<<endl;
        reportErrors();
        return true;
    }
    if((cmdList.size() == 2) && 
        (cmdList[0] == "check") && (cmdList[1] == "state"))
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
                cout<<OKGREEN<<"<RUNNING> ";
            }
            else
                cout<<FAIL<<"<STOPPED> ";
            cout<<INFO<<"("<<id<<") ";
            cout<<(*moditr)->getCommand();
            cout<<" ["<<(*moditr)->getHost()<<"]"<<ENDC<<endl;
            id++;
        }

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
        unsigned int id = (unsigned int)atoi(cmdList[2].c_str());
        if(id>=connections.size())
        {
            cout<<FAIL<<"ERROR:   "<<INFO<<"Connection id is out of range."<<ENDC<<endl;
            return true;
        }

        if(connected(id))
            cout<<OKGREEN<<"<CONNECTED> ";
        else
            cout<<FAIL<<"<DISCONNECTED> ";

        cout<<INFO<<"("<<id<<") ";
        cout<<connections[id].from()<<" - "<<connections[id].to();
                cout<<" ["<<connections[id].carrier()<<"]"<<ENDC<<endl;
        reportErrors();
        return true;
    }
    if((cmdList.size() == 2) && 
        (cmdList[0] == "check") && (cmdList[1] == "con"))
    {
        CnnContainer connections  = getConnections();
        CnnIterator cnnitr;
        int id = 0;
        for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
        {
            if(connected(id))
                cout<<OKGREEN<<"<CONNECTED> ";
            else
                cout<<FAIL<<"<DISCONNECTED> ";

            cout<<INFO<<"("<<id<<") ";
            cout<<(*cnnitr).from()<<" - "<<(*cnnitr).to();
                cout<<" ["<<(*cnnitr).carrier()<<"]"<<ENDC<<endl;
            id++;
        }
        return true;
    }



    /**
     *  list available modules
     */
    if((cmdList.size() == 2) && 
        (cmdList[0] == "list") && (cmdList[1] == "mod"))
    {
        KnowledgeBase* kb = getKnowledgeBase();
        ModulePContainer mods =  kb->getModules();
        int id = 0;
        for(ModulePIterator itr=mods.begin(); itr!=mods.end(); itr++)
        {
            string fname;
            string fpath = (*itr)->getXmlFile();

            size_t pos = fpath.rfind(PATH_SEPERATOR);
            if(pos!=string::npos)
                fname = fpath.substr(pos);
            else
                fname = fpath;
            cout<<INFO<<"("<<id++<<") ";
            cout<<OKBLUE<<(*itr)->getName()<<ENDC;
            cout<<INFO<<" ["<<fname<<"]"<<ENDC<<endl;
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
        for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
        {
            string fname;
            string fpath = (*itr)->getXmlFile();

            size_t pos = fpath.rfind(PATH_SEPERATOR);
            if(pos!=string::npos)
                fname = fpath.substr(pos);
            else
                fname = fpath;
            cout<<INFO<<"("<<id++<<") ";
            cout<<OKBLUE<<(*itr)->getName()<<ENDC;
            cout<<INFO<<" ["<<fname<<"]"<<ENDC<<endl;
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
        for(ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++)
        {
            Computer* comp = dynamic_cast<Computer*>(*itr);
            if(comp)
            {
                string fname;
                string fpath = comp->getXmlFile();
                size_t pos = fpath.rfind(PATH_SEPERATOR);
                if(pos!=string::npos)
                    fname = fpath.substr(pos);
                else
                    fname = fpath;
                cout<<INFO<<"("<<id++<<") ";
                if(comp->getDisable())
                    cout<<WARNING<<comp->getName()<<ENDC;
                else
                    cout<<OKBLUE<<comp->getName()<<ENDC;
                cout<<INFO<<" ["<<fname<<"]"<<ENDC<<endl;
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
        if(!exportDependencyGraph(cmdList[1].c_str()))
            cout<<FAIL<<"ERROR:   "<<INFO<<"Cannot export graph to "<<cmdList[1]<<"."<<ENDC<<endl;
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
            cout<<FAIL<<"ERROR:   "<<INFO<<"'"<<cmdList[2].c_str()<<"' not found."<<ENDC<<endl;
            return true;
        }
        cout<<INFO;
        PRINT_MODULE(kb->getModule(cmdList[2].c_str()));
        cout<<ENDC;
        return true;
    }   

    /**
     * set an option  
     */ 
     if((cmdList.size() == 3) && 
        (cmdList[0] == "set"))
     {
         config.unput(cmdList[1].c_str());
         config.put(cmdList[1].c_str(), cmdList[2].c_str());
         
        if(cmdList[1] == string("watchdog"))
        {
            if(cmdList[2] == string("yes"))
                enableWatchDog();
            else
                disableWatchod();
        }

        if(cmdList[1] == string("auto_dependency"))
        {
            if(cmdList[2] == string("yes"))
                enableAutoDependency();
            else
                disableAutoDependency();
        }

        if(cmdList[1] == string("auto_connect"))
        {
            if(cmdList[2] == string("yes"))
                enableAutoConnect();
            else
                disableAutoConnect();
        }
    
        if(cmdList[1] == string("color_theme"))
        {
            if(cmdList[2] == string("dark"))
                setColorTheme(THEME_DARK);
            else if(cmdList[2] == string("light"))
                setColorTheme(THEME_LIGHT);
            else
                setColorTheme(THEME_NONE);
        }

        return true;
     }


    /**
     * get an option
     */ 
     if((cmdList.size() == 2) && 
        (cmdList[0] == "get"))
     {
         if(config.check(cmdList[1].c_str()))
         {
            cout<<OKBLUE<<cmdList[1]<<INFO<<" = ";
            cout<<OKGREEN<<config.find(cmdList[1].c_str()).asString()<<ENDC<<endl;
         }
         else
            cout<<FAIL<<"ERROR:   "<<INFO<<"'"<<cmdList[1].c_str()<<"' not found."<<ENDC<<endl;
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


void YConsoleManager::help(void)
{
    cout<<"Here is a list of Yarp manager keywords.\n"<<endl;   
    cout<<OKGREEN<<"help"<<INFO<<"                    : show help."<<ENDC<<endl;
    cout<<OKGREEN<<"exit"<<INFO<<"                    : exit yarp manager."<<ENDC<<endl;
    cout<<OKGREEN<<"list mod"<<INFO<<"                : list available modules."<<ENDC<<endl;
    cout<<OKGREEN<<"list app"<<INFO<<"                : list available applications."<<ENDC<<endl;
    cout<<OKGREEN<<"list res"<<INFO<<"                : list available resources. (i.e. nodes in a cluster)"<<ENDC<<endl;
    cout<<OKGREEN<<"add mod <filename>"<<INFO<<"      : add a module from its description file."<<ENDC<<endl;
    cout<<OKGREEN<<"add app <filename>"<<INFO<<"      : add an application from its description file."<<ENDC<<endl;
    cout<<OKGREEN<<"add res <filename>"<<INFO<<"      : add resources from a description file."<<ENDC<<endl;
    cout<<OKGREEN<<"load app <application>"<<INFO<<"  : load an application to run."<<endl;
//  cout<<"load mod <module> <host>: load a module to run on an optional host."<<endl;
    cout<<OKGREEN<<"run [IDs]"<<INFO<<"               : run application or a modules indicated by IDs."<<ENDC<<endl;
    cout<<OKGREEN<<"stop [IDs]"<<INFO<<"              : stop running application or modules indicated by IDs."<<ENDC<<endl;
    cout<<OKGREEN<<"kill [IDs]"<<INFO<<"              : kill running application or modules indicated by IDs."<<ENDC<<endl;
    cout<<OKGREEN<<"connect [IDs]"<<INFO<<"           : stablish all connections or just one connection indicated by IDs."<<ENDC<<endl;
    cout<<OKGREEN<<"disconnect [IDs]"<<INFO<<"        : remove all connections or just one connection indicated by IDs."<<ENDC<<endl;
    cout<<OKGREEN<<"which"<<INFO<<"                   : list loaded modules, connections and resource dependencies."<<ENDC<<endl;   
    cout<<OKGREEN<<"check dep"<<INFO<<"               : check for all resource dependencies."<<ENDC<<endl;
    cout<<OKGREEN<<"check state [id]"<<INFO<<"        : check for running state of application or a module indicated by id."<<ENDC<<endl;
    cout<<OKGREEN<<"check con [id]"<<INFO<<"          : check for all connections state or just one connection indicated by id."<<ENDC<<endl;
    cout<<OKGREEN<<"set <option> <value>"<<INFO<<"    : set value to an option."<<ENDC<<endl;
    cout<<OKGREEN<<"get <option>"<<INFO<<"            : show value of an option."<<ENDC<<endl;  
    cout<<OKGREEN<<"export <filename>"<<INFO<<"       : export application's graph as Graphviz dot format."<<ENDC<<endl;
//  cout<<"edit mod <modname>      : open module relevant xml file to edit."<<endl;
//  cout<<"edit app <appname>      : open application relevant xml file to edit."<<endl;
    cout<<OKGREEN<<"show mod <modname>"<<INFO<<"      : display module information (description, input, output,...)."<<ENDC<<endl;
    cout<<OKGREEN<<"assign hosts"<<INFO<<"            : automatically assign modules to proper nodes using load balancer."<<ENDC<<endl;
   
    cout<<endl;
}


void YConsoleManager::which(void)
{
    ExecutablePContainer modules = getExecutables();
    CnnContainer connections  = getConnections();
    ExecutablePIterator moditr;
    CnnIterator cnnitr;
    
    cout<<endl<<HEADER<<"Application: "<<ENDC<<endl;
    cout<<OKBLUE<<getApplicationName()<<ENDC<<endl;

    cout<<endl<<HEADER<<"Modules: "<<ENDC<<endl;
    int id = 0;
    for(moditr=modules.begin(); moditr<modules.end(); moditr++)
    {
        cout<<INFO<<"("<<id++<<") ";
        cout<<OKBLUE<<(*moditr)->getCommand()<<INFO;
        cout<<" ["<<(*moditr)->getHost()<<"] ["<<(*moditr)->getParam()<<"]";
        cout<<" ["<<(*moditr)->getEnv()<<"]"<<ENDC<<endl; 
    }
    cout<<endl<<HEADER<<"Connections: "<<ENDC<<endl;
    id = 0;
    for(cnnitr=connections.begin(); cnnitr<connections.end(); cnnitr++)
    {
        cout<<INFO<<"("<<id++<<") ";
        cout<<OKBLUE<<(*cnnitr).from()<<" - "<<(*cnnitr).to()<<INFO;
            cout<<" ["<<(*cnnitr).carrier()<<"]";
        cout<<ENDC<<endl;
    }       
    
    cout<<endl<<HEADER<<"Resources:"<<ENDC<<endl;
    id = 0;
    ResourcePIterator itrS;
    for(itrS=getResources().begin(); itrS!=getResources().end(); itrS++)
    {
        cout<<INFO<<"("<<id++<<") ";
        cout<<OKBLUE<<(*itrS)->getName()<<INFO<<" ["<<(*itrS)->getTypeName()<<"]"<<ENDC<<endl;
    }
    cout<<endl;
}


void YConsoleManager::reportErrors(void)
{
    ErrorLogger* logger  = ErrorLogger::Instance(); 
    if(logger->errorCount() || logger->warningCount())
    {
        const char* msg;
        while((msg=logger->getLastError()))
            cout<<FAIL<<"ERROR:   "<<INFO<<msg<<ENDC<<endl;

        while((msg=logger->getLastWarning()))
            cout<<WARNING<<"WARNING: "<<INFO<<msg<<ENDC<<endl;
    }   
}
    

void YConsoleManager::onExecutableStart(void* which) { }

void YConsoleManager::onExecutableStop(void* which) { }

void YConsoleManager::onExecutableDied(void* which) { }

void YConsoleManager::onExecutableFailed(void* which)
{
    Executable* exe = (Executable*) which;
    if(config.find("module_failure").asString() == "prompt")
        cout<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed!"<<endl;
    
    if(config.find("module_failure").asString() == "recover")
    {
        cout<<endl<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! (restarting...)"<<endl;
        exe->start();
    }

    if(config.find("module_failure").asString() == "terminate")
    {
        cout<<endl<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! (terminating application...)"<<endl;  
        bShouldRun = false;
        stop();
        reportErrors();
    }
}

void YConsoleManager::onCnnStablished(void* which) { }

void YConsoleManager::onCnnFailed(void* which) 
{
    Connection* cnn = (Connection*) which;
    if(config.check("connection_failure") &&
     config.find("connection_failure").asString() == "prompt")
        cout<<endl<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<endl;

    if(bShouldRun && config.check("connection_failure") &&
     config.find("connection_failure").asString() == "terminate")
     {
        cout<<endl<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<"(terminating application...)"<<endl;
        bShouldRun = false;
        stop();
        reportErrors();
     }
}


bool YConsoleManager::loadRecursiveApplications(const char* szPath)
{
    string strPath = szPath;
    if((strPath.rfind(PATH_SEPERATOR)==string::npos) || 
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(strPath.c_str())) == NULL)
        return false;

    addApplications(strPath.c_str());

    while((entry = readdir(dir)))
    {
        if((string(entry->d_name) != string(".")) 
        && (string(entry->d_name) != string("..")))
        {
            string name = strPath + string(entry->d_name);
            loadRecursiveApplications(name.c_str());
        }
    }
    closedir(dir);
    return true;
}



void YConsoleManager::updateAppNames(vector<string>* names)
{
    names->clear();
    KnowledgeBase* kb = getKnowledgeBase();
    ApplicaitonPContainer apps =  kb->getApplications();
    for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
        names->push_back((*itr)->getName());    
}



void YConsoleManager::setColorTheme(ColorTheme theme)
{

#if defined(WIN32)
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



#ifdef WITH_READLINE

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

    matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
    if (start == 0)
        matches = rl_completion_matches(text, &command_generator);
   else
        matches = rl_completion_matches(text, &appname_generator);

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
      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }
  
  /* if no names matched, then return null. */
  return ((char *)NULL);
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
      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }
  
  return ((char *)NULL);
}

#endif 
