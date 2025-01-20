/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "safe_manager.h"

using namespace yarp::manager;

#define WAIT_SEMAPHOR() waitSemaphore();

#define POST_SEMAPHOR() postSemaphore();


SafeManager::SafeManager() :
    m_pConfig(nullptr),
    action(MNOTHING),
    eventReceiver(nullptr),
    busyAction(false)
{}

SafeManager::~SafeManager() = default;

void SafeManager::close() {
    yarp::os::Thread::stop();
    WAIT_SEMAPHOR();
    eventReceiver = nullptr;
    POST_SEMAPHOR();
}

bool SafeManager::prepare(Manager* lazy,
                         yarp::os::Property* pConfig, ApplicationEvent* event)
{

    eventReceiver = event;
    m_pConfig = pConfig;

    if (pConfig->find("watchdog").asString() == "yes") {
        enableWatchDog();
    } else {
        disableWatchod();
    }

    if (pConfig->find("auto_dependency").asString() == "yes") {
        enableAutoDependency();
    } else {
        disableAutoDependency();
    }

    if (pConfig->find("auto_connect").asString() == "yes") {
        enableAutoConnect();
    } else {
        disableAutoConnect();
    }

    // making manager from lazy manager
    KnowledgeBase* lazy_kb = lazy->getKnowledgeBase();

    ModulePContainer mods =  lazy_kb->getModules();
    for (auto& mod : mods) {
        getKnowledgeBase()->addModule(mod);
    }

    ResourcePContainer res =  lazy_kb->getResources();
    for (auto& re : res) {
        getKnowledgeBase()->addResource(re);
    }

    ApplicaitonPContainer apps =  lazy_kb->getApplications();
    for (auto& app : apps) {
        getKnowledgeBase()->addApplication(app);
    }

    return true;
}



bool SafeManager::threadInit() {
    return true;
}

void SafeManager::threadRelease()
{
}

bool SafeManager::busy() {
    bool ret;
    WAIT_SEMAPHOR();
    ret =  busyAction;
    POST_SEMAPHOR();
    return ret;
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
            std::vector<double> waitVec;
            for (int local_modId : local_modIds)
            {
                Executable * exec = Manager::getExecutableById(local_modId);
                if (exec)
                {
                    waitVec.push_back(exec->getPostExecWait());
                }
            }
            double minWait=*std::min_element(waitVec.begin(), waitVec.end());
            for (int local_modId : local_modIds)
            {
                Executable * exec = Manager::getExecutableById(local_modId);
                if (exec)
                {
                    exec->setPostExecWait(exec->getPostExecWait() - minWait);
                }
                Manager::run(local_modId, true);
            }

            /*
            for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::waitingModuleRun(local_modIds[i]);

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
            }*/
            break;
        }
    case MSTOP:{
            std::vector<double> waitVec;
            for (int local_modId : local_modIds)
            {
                Executable * exec = Manager::getExecutableById(local_modId);
                if (exec)
                {
                    waitVec.push_back(exec->getPostStopWait());
                }
            }
            double minWait=*std::min_element(waitVec.begin(), waitVec.end());
            for (int local_modId : local_modIds)
            {
                Executable * exec = Manager::getExecutableById(local_modId);
                if (exec)
                {
                    exec->setPostStopWait(exec->getPostStopWait() - minWait);
                }
                Manager::stop(local_modId, true);
            }
            /*for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::waitingModuleStop(local_modIds[i]);

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
            }*/
            break;
        }
    case MKILL:{
        for (int local_modId : local_modIds) {
            Manager::kill(local_modId, true);
        }
            /*for(unsigned int i=0; i<local_modIds.size(); i++)
                Manager::waitingModuleKill(local_modIds[i]);

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
            }*/
            break;
        }
    case MCONNECT:{
            for(int local_conId : local_conIds)
            {
                refreshPortStatus(local_conId);
                if(Manager::connect(local_conId))
                {
                    if (eventReceiver) {
                        eventReceiver->onConConnect(local_conId);
                    }
                }
                else
                {
                    if (eventReceiver) {
                        eventReceiver->onConDisconnect(local_conId);
                    }
                }
            }
            break;
        }
    case MDISCONNECT:{
            for(int local_conId : local_conIds)
            {
                refreshPortStatus(local_conId);
                if(Manager::disconnect(local_conId))
                {
                    if (eventReceiver) {
                        eventReceiver->onConDisconnect(local_conId);
                    }
                }
                else
                {
                    if (eventReceiver) {
                        eventReceiver->onConConnect(local_conId);
                    }
                }
            }
            break;
        }

    case MREFRESH:{
            busyAction = true;

            for(int local_modId : local_modIds)
            {
                if(Manager::running(local_modId))
                {
                    if (eventReceiver) {
                        eventReceiver->onModStart(local_modId);
                    }
                }
                else //if(Manager::suspended(local_modIds[i]))
                {
                    if (eventReceiver) {
                        eventReceiver->onModStop(local_modId);
                    }
                }
            }

            for(int local_conId : local_conIds)
            {
                refreshPortStatus(local_conId);
                if(Manager::connected(local_conId))
                {
                    if (eventReceiver) {
                        eventReceiver->onConConnect(local_conId);
                    }
                }
                else
                {
                    if (eventReceiver) {
                        eventReceiver->onConDisconnect(local_conId);
                    }
                }
            }

            for(int local_resId : local_resIds)
            {
                if(Manager::exist(local_resId))
                {
                    if (eventReceiver) {
                        eventReceiver->onResAvailable(local_resId);
                    }
                }
                else
                {
                    if (eventReceiver) {
                        eventReceiver->onResUnAvailable(local_resId);
                    }
                }
            }
            busyAction = false;
            break;
        }

    case MREFRESH_CNN:{
            for(int local_conId : local_conIds)
            {
                refreshPortStatus(local_conId);
                if(Manager::connected(local_conId))
                {
                    if (eventReceiver) {
                        eventReceiver->onConConnect(local_conId);
                    }
                }
                else
                {
                    if (eventReceiver) {
                        eventReceiver->onConDisconnect(local_conId);
                    }
                }
            }
            break;
        }

    case MATTACHSTDOUT:{
        for (int local_modId : local_modIds) {
            Manager::attachStdout(local_modId);
        }
            break;
        }

    case MDETACHSTDOUT:{
        for (int local_modId : local_modIds) {
            Manager::detachStdout(local_modId);
        }
            break;
        }

    case MLOADBALANCE:{
                busyAction = true;
                Manager::loadBalance();
                if (eventReceiver) {
                    eventReceiver->onLoadBalance();
                }
                busyAction = false;
            break;
        }


    default:
        break;
    };

    if (eventReceiver) {
        eventReceiver->onError();
    }
}

void SafeManager::safeRun(std::vector<int>& MIDs, std::vector<int>& CIDs, std::vector<int> &RIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    modIds = MIDs;
    conIds = CIDs;
    resIds = RIDs;
    action = MRUN;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}

void SafeManager::safeStop(std::vector<int>& MIDs, std::vector<int>& CIDs, std::vector<int> &RIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    modIds = MIDs;
    conIds = CIDs;
    resIds = RIDs;
    action = MSTOP;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}

void SafeManager::safeKill(std::vector<int>& MIDs, std::vector<int> &CIDs, std::vector<int> &RIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    modIds = MIDs;
    conIds = CIDs;
    resIds = RIDs;
    action = MKILL;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}


void SafeManager::safeConnect(std::vector<int>& CIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    conIds = CIDs;
    action = MCONNECT;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}


void SafeManager::safeDisconnect(std::vector<int>& CIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    conIds = CIDs;
    action = MDISCONNECT;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}


void SafeManager::safeRefresh(std::vector<int>& MIDs,
                     std::vector<int>& CIDs,
                     std::vector<int>& RIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    modIds = MIDs;
    conIds = CIDs;
    resIds = RIDs;
    action = MREFRESH;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}


void SafeManager::safeAttachStdout(std::vector<int>& MIDs)
{
    if (busy()) {
        return;
    }
    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MATTACHSTDOUT;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}

void SafeManager::safeDetachStdout(std::vector<int>& MIDs)
{
    if (busy()) {
        return;
    }

    WAIT_SEMAPHOR();
    modIds = MIDs;
    action = MDETACHSTDOUT;
    POST_SEMAPHOR();
    if (!yarp::os::Thread::isRunning()) {
        yarp::os::Thread::start();
    }
}

void SafeManager::safeLoadBalance()
{
    if (busy()) {
        return;
    }

   WAIT_SEMAPHOR();
   action = MLOADBALANCE;
   POST_SEMAPHOR();
   if (!yarp::os::Thread::isRunning()) {
       yarp::os::Thread::start();
   }
}

void SafeManager::onExecutableStart(void* which)
{
    WAIT_SEMAPHOR();
    auto* exe = static_cast<Executable*>(which);
    if (eventReceiver && exe) {
        eventReceiver->onModStart(exe->getID());
    }
    POST_SEMAPHOR();
}

void SafeManager::onExecutableStop(void* which)
{
    WAIT_SEMAPHOR();
    auto* exe = static_cast<Executable*>(which);
    if (eventReceiver && exe) {
        eventReceiver->onModStop(exe->getID());
    }
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
    auto* exe = static_cast<Executable*>(which);
    if (eventReceiver && exe) {
        eventReceiver->onModStop(exe->getID());
    }
    POST_SEMAPHOR();
}


void SafeManager::onExecutableFailed(void* which)
{
    WAIT_SEMAPHOR();
    ErrorLogger* logger  = ErrorLogger::Instance();
    auto* exe = static_cast<Executable*>(which);
    if(exe)
    {
        if(m_pConfig->find("module_failure").asString() == "prompt")
        {
            OSTRINGSTREAM err;
            err<<exe->getCommand()<<" from "<<exe->getHost()<<" is failed! [id:"<<exe->getID()<<"]";
            logger->addError(err);
            if (eventReceiver && exe) {
                eventReceiver->onModStop(exe->getID());
            }
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

    if (eventReceiver) {
        eventReceiver->onError();
    }
    POST_SEMAPHOR();
}


void SafeManager::onCnnStablished(void* which) { }


void SafeManager::onCnnFailed(void* which)
{
    WAIT_SEMAPHOR();
    ErrorLogger* logger  = ErrorLogger::Instance();
    auto* cnn = static_cast<Connection*>(which);
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

    if (eventReceiver) {
        eventReceiver->onError();
    }
    POST_SEMAPHOR();
}


void SafeManager::onExecutableStdout(void* which, const char* msg)
{
    WAIT_SEMAPHOR();
    auto* exe = static_cast<Executable*>(which);
    if (eventReceiver) {
        eventReceiver->onModStdout(exe->getID(), msg);
    }
    POST_SEMAPHOR();
}

void SafeManager::onError(void* which)
{
    WAIT_SEMAPHOR();
    if (eventReceiver) {
        eventReceiver->onError();
    }
    POST_SEMAPHOR();
}

void SafeManager::refreshPortStatus(int id)
{
    // refreshing ports status
    if(Manager::existPortFrom(id))
    {
        if (eventReceiver) {
            eventReceiver->onConAvailable(id, -1);
        }
    }
    else
    {
        if (eventReceiver) {
            eventReceiver->onConUnAvailable(id, -1);
        }
    }

    if(Manager::existPortTo(id))
    {
        if (eventReceiver) {
            eventReceiver->onConAvailable(-1, id);
        }
    }
    else
    {
        if (eventReceiver) {
            eventReceiver->onConUnAvailable(-1, id);
        }
    }
}
