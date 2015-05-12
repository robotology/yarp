/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "safe_manager.h"

using namespace yarp::manager;

#define WAIT_SEMAPHOR() waitSemaphore();

#define POST_SEMAPHOR() postSemaphore();


SafeManager::SafeManager()
    :action(MNOTHING), eventReceiver(NULL), busyAction(false)
{ }

SafeManager::~SafeManager() { }

void SafeManager::close() {
    yarp::os::Thread::stop();
    WAIT_SEMAPHOR();
    eventReceiver = NULL;
    POST_SEMAPHOR();
}

bool SafeManager::prepare(Manager* lazy,
                         yarp::os::Property* pConfig, ApplicationEvent* event)
{

    eventReceiver = event;
    m_pConfig = pConfig;

    if(pConfig->find("watchdog").asString() == "yes")
        enableWatchDog();
    else
        disableWatchod();

    if(pConfig->find("auto_dependency").asString() == "yes")
        enableAutoDependency();
    else
        disableAutoDependency();

    if(pConfig->find("auto_connect").asString() == "yes")
        enableAutoConnect();
    else
        disableAutoConnect();

    // making manager from lazy manager
    KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();

    ModulePContainer mods =  lazy_kb->getModules();
    for(ModulePIterator itr=mods.begin(); itr!=mods.end(); itr++)
        getKnowledgeBase()->addModule((*itr));

    ResourcePContainer res =  lazy_kb->getResources();
    for(ResourcePIterator itr=res.begin(); itr!=res.end(); itr++)
        getKnowledgeBase()->addResource((*itr));

    ApplicaitonPContainer apps =  lazy_kb->getApplications();
    for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
        getKnowledgeBase()->addApplication((*itr));

    return true;
}



bool SafeManager::threadInit() {
    return true;
}

void SafeManager::threadRelease()
{
}

void SafeManager::run()
{
    WAIT_SEMAPHOR();
    ThreadAction local_action = action;
    std::vector<int> local_modIds = modIds;
    std::vector<int> local_conIds = conIds;
    std::vector<int> local_resIds = resIds;
    POST_SEMAPHOR();

    switch(local_action){
    case MRUN:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::run(local_modIds[i], true);
            break;
        }
    case MSTOP:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::stop(local_modIds[i], true);
            break;
        }
    case MKILL:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::kill(local_modIds[i], true);
            break;
        }
    case MCONNECT:{
            for(unsigned int i=0; i<local_conIds.size(); i++)
            {
                if(Manager::connect(local_conIds[i]))
                {
                    if(eventReceiver) eventReceiver->onConConnect(local_conIds[i]);
                }
                else
                {
                    if(eventReceiver) eventReceiver->onConDisconnect(local_conIds[i]);
                }
                refreshPortStatus(local_conIds[i]);
            }
            break;
        }
    case MDISCONNECT:{
            for(unsigned int i=0; i<local_conIds.size(); i++)
            {
                if(Manager::disconnect(local_conIds[i]))
                {
                    if(eventReceiver) eventReceiver->onConDisconnect(local_conIds[i]);
                }
                else
                {
                    if(eventReceiver) eventReceiver->onConConnect(local_conIds[i]);
                }
                refreshPortStatus(local_conIds[i]);
            }
            break;
        }

    case MREFRESH:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
            {
                if(Manager::running(local_modIds[i]))
                {
                    if(eventReceiver) eventReceiver->onModStart(local_modIds[i]);
                }
                else //if(Manager::suspended(local_modIds[i]))
                {
                    if(eventReceiver) eventReceiver->onModStop(local_modIds[i]);
                }
            }

            for(unsigned int i=0; i<local_conIds.size(); i++)
            {
                if(Manager::connected(local_conIds[i]))
                {
                    if(eventReceiver) eventReceiver->onConConnect(local_conIds[i]);
                }
                else
                {
                    if(eventReceiver) eventReceiver->onConDisconnect(local_conIds[i]);
                }
                refreshPortStatus(local_conIds[i]);
            }

            for(unsigned int i=0; i<local_resIds.size(); i++)
            {
                if(Manager::exist(local_resIds[i]))
                {
                    if(eventReceiver) eventReceiver->onResAvailable(local_resIds[i]);
                }
                else
                {
                    if(eventReceiver) eventReceiver->onResUnAvailable(local_resIds[i]);
                }
            }
            busyAction = false;
            break;
        }

    case MREFRESH_CNN:{
            for(unsigned int i=0; i<local_conIds.size(); i++)
            {
                if(Manager::connected(local_conIds[i]))
                {
                    if(eventReceiver) eventReceiver->onConConnect(local_conIds[i]);
                }
                else
                {
                    if(eventReceiver) eventReceiver->onConDisconnect(local_conIds[i]);
                }
                refreshPortStatus(local_conIds[i]);
            }
        }

    case MATTACHSTDOUT:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::attachStdout(local_modIds[i]);
            break;
        }

    case MDETACHSTDOUT:{
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::detachStdout(local_modIds[i]);
            break;
        }

    case MLOADBALANCE:{
                Manager::loadBalance();
                if(eventReceiver) eventReceiver->onLoadBalance();
                busyAction = false;
            break;
        }


    default:
        break;
    };

    if(eventReceiver)
        eventReceiver->onError();
}

void SafeManager::safeRun(std::vector<int>& MIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MRUN;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}

void SafeManager::safeStop(std::vector<int>& MIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MSTOP;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}

void SafeManager::safeKill(std::vector<int>& MIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MKILL;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();

}


void SafeManager::safeConnect(std::vector<int>& CIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    conIds = CIDs;
    action = MCONNECT;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}


void SafeManager::safeDisconnect(std::vector<int>& CIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    conIds = CIDs;
    action = MDISCONNECT;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}


void SafeManager::safeRefresh(std::vector<int>& MIDs,
                     std::vector<int>& CIDs,
                     std::vector<int>& RIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    conIds = CIDs;
    resIds = RIDs;
    action = MREFRESH;
    busyAction = true;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}


void SafeManager::safeAttachStdout(std::vector<int>& MIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MATTACHSTDOUT;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}

void SafeManager::safeDetachStdout(std::vector<int>& MIDs)
{
    if(busyAction) return;
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MDETACHSTDOUT;
    POST_SEMAPHOR();
    if(!yarp::os::Thread::isRunning())
        yarp::os::Thread::start();
}

void SafeManager::safeLoadBalance(void)
{
   if(busyAction) return;
   WAIT_SEMAPHOR();
   action = MLOADBALANCE;
   busyAction = true;
   POST_SEMAPHOR();
   if(!yarp::os::Thread::isRunning())
       yarp::os::Thread::start();
}

void SafeManager::onExecutableStart(void* which)
{
    WAIT_SEMAPHOR();
    Executable* exe = static_cast<Executable*>(which);
    if(eventReceiver && exe)
        eventReceiver->onModStart(exe->getID());
    POST_SEMAPHOR();
}

void SafeManager::onExecutableStop(void* which)
{
    WAIT_SEMAPHOR();
    Executable* exe = static_cast<Executable*>(which);
    if(eventReceiver && exe)
        eventReceiver->onModStop(exe->getID());
    POST_SEMAPHOR();
    // Experimental:
    //  do auto refresh on connections whenever a module stops
    /*
    if(checkSemaphore())
    {
        if(!isRunning())
        {
            conIds.clear();
            for(int i=0; i<getConnections().size(); i++)
                conIds.push_back(i);
            action = MREFRESH_CNN;
            yarp::os::Thread::start();
        }
        POST_SEMAPHOR();
    }
    */
}

void SafeManager::onExecutableDied(void* which)
{
    WAIT_SEMAPHOR();
    Executable* exe = static_cast<Executable*>(which);
    if(eventReceiver && exe)
        eventReceiver->onModStop(exe->getID());
    POST_SEMAPHOR();
}


void SafeManager::onExecutableFailed(void* which)
{
    WAIT_SEMAPHOR();
    ErrorLogger* logger  = ErrorLogger::Instance();
    Executable* exe = static_cast<Executable*>(which);
    if(exe)
    {
        if(m_pConfig->find("module_failure").asString() == "prompt")
        {
            OSTRINGSTREAM err;
            err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"]";
            logger->addError(err);
            if(eventReceiver && exe)
                eventReceiver->onModStop(exe->getID());
        }

        if(m_pConfig->find("module_failure").asString() == "recover")
        {
            OSTRINGSTREAM err;
            err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (restarting...)";
            logger->addError(err);
            exe->start();
         }

        if(m_pConfig->find("module_failure").asString() == "terminate")
        {
            OSTRINGSTREAM err;
            err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"] (terminating...)";
            logger->addError(err);
            Manager::stop();
        }
    }

    if(eventReceiver)
            eventReceiver->onError();
    POST_SEMAPHOR();
}


void SafeManager::onCnnStablished(void* which) { }


void SafeManager::onCnnFailed(void* which)
{
    WAIT_SEMAPHOR();
    ErrorLogger* logger  = ErrorLogger::Instance();
    Connection* cnn = static_cast<Connection*>(which);
    if(cnn)
    {
        if( m_pConfig->find("connection_failure").asString() == "prompt")
        {
            OSTRINGSTREAM err;
            err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to();
            logger->addError(err);
        }

        if(m_pConfig->find("connection_failure").asString() == "terminate")
        {
            OSTRINGSTREAM err;
            err<<"connection failed between "<<cnn->from()<<" and "<<cnn->to()<<" (terminating...)";
            logger->addError(err);
            Manager::stop();
        }
    }

    if(eventReceiver)
        eventReceiver->onError();
    POST_SEMAPHOR();
}


void SafeManager::onExecutableStdout(void* which, const char* msg)
{
    WAIT_SEMAPHOR();
    Executable* exe = static_cast<Executable*>(which);
    if(eventReceiver)
        eventReceiver->onModStdout(exe->getID(), msg);
    POST_SEMAPHOR();
}

void SafeManager::onError(void* which)
{
    WAIT_SEMAPHOR();
    if(eventReceiver)
        eventReceiver->onError();
    POST_SEMAPHOR();
}

void SafeManager::refreshPortStatus(int id)
{
    // refreshing ports status
    if(Manager::existPortFrom(id))
    {
        if(eventReceiver) eventReceiver->onConAvailable(id, -1);
    }
    else
    {
        if(eventReceiver) eventReceiver->onConUnAvailable(id, -1);
    }

    if(Manager::existPortTo(id))
    {
        if(eventReceiver) eventReceiver->onConAvailable(-1, id);
    }
    else
    {
        if(eventReceiver) eventReceiver->onConUnAvailable(-1, id);
    }
}

