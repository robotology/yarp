/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "manager.h"
#include "yarpbroker.h"
#include "localbroker.h"
#include "yarpdevbroker.h"
#include "xmlapploader.h"
#include "xmlmodloader.h"
#include "xmlresloader.h"
#include "xmlappsaver.h"
#include "singleapploader.h"

#define RUN_TIMEOUT             10      // Run timeout in seconds
#define STOP_TIMEOUT            30      // Stop timeout in seconds
#define KILL_TIMEOUT            10      // kill timeout in seconds

#define BROKER_LOCAL            "local"
#define BROKER_YARPRUN          "yarprun"
#define BROKER_YARPDEV          "yarpdev"
#define BROKER_ICUBMIODDEV      "icubmoddev"


/**
 * Class Manager
 */

Manager::Manager(bool withWatchDog) : MEvent()
{
    logger  = ErrorLogger::Instance();
    bWithWatchDog = withWatchDog;
    bAutoDependancy = false;
    bRestricted = false;
    strDefBroker = BROKER_YARPRUN;
    knowledge.createFrom(NULL, NULL, NULL);
    connector.init();
}

Manager::Manager(const char* szModPath, const char* szAppPath,
                 const char* szResPath, bool withWatchDog)
{
    logger  = ErrorLogger::Instance();
    bWithWatchDog = withWatchDog;
    bAutoDependancy = false;
    bRestricted = false;
    strDefBroker = BROKER_YARPRUN;

    XmlModLoader modload(szModPath, NULL);
    XmlModLoader* pModLoad = &modload;
    if(!modload.init())
        pModLoad = NULL;

    XmlAppLoader appload(szAppPath, NULL);
    XmlAppLoader* pAppLoad = &appload;
    if(!appload.init())
        pAppLoad = NULL;

    XmlResLoader resload(szResPath, NULL);
    XmlResLoader* pResLoad = &resload;
    if(!resload.init())
        pResLoad = NULL;

    knowledge.createFrom(pModLoad, pAppLoad, pResLoad);
    connector.init();
}


Manager::~Manager()
{
    // untopic persistent connections
    rmconnect();
    clearExecutables();
}


bool Manager::addApplication(const char* szFileName, char* szAppName_)
{
    XmlAppLoader appload(szFileName);
    if(!appload.init())
        return false;
    Application* application = appload.getNextApplication();
    if(!application)
        return false;
    return knowledge.addApplication(application, szAppName_);
}


bool Manager::addApplications(const char* szPath)
{
    XmlAppLoader appload(szPath, NULL);
    if(!appload.init())
        return false;
    Application* application;
    while((application = appload.getNextApplication()))
        knowledge.addApplication(application);
    return true;
}


bool Manager::addModule(const char* szFileName)
{
    XmlModLoader modload(szFileName);
    if(!modload.init())
        return false;
    Module* module = modload.getNextModule();
    if(!module)
        return false;
    return knowledge.addModule(module);
}


bool Manager::addModules(const char* szPath)
{
    XmlModLoader modload(szPath, NULL);
    if(!modload.init())
        return false;
    Module* module;
    while((module = modload.getNextModule()))
        knowledge.addModule(module);
    return true;
}


bool Manager::addResource(const char* szFileName)
{
    XmlResLoader resload(szFileName);
    if(!resload.init())
        return false;
    GenericResource* resource;
    bool bloaded = false;
    while((resource = resload.getNextResource()))
           bloaded |= knowledge.addResource(resource);
    return bloaded;
}


bool Manager::addResources(const char* szPath)
{
    XmlResLoader resload(szPath, NULL);
    if(!resload.init())
        return false;
    GenericResource* resource;
    while((resource = resload.getNextResource()))
        knowledge.addResource(resource);
    return true;
}


bool Manager::removeApplication(const char* szAppName)
{
    //Note: use it with care. it is better we first check that no application
    //is loaded.
    if(!runnables.empty())
    {
        logger->addError("Application cannot be removed if there is a loaded application");
        return false;
    }

    Application* app = knowledge.getApplication(szAppName);
    if(!app)
        return false;

    return knowledge.removeApplication(app);
}


bool Manager::removeModule(const char* szModName)
{
    //Note: use it with care. it is better we first check that no application
    //is loaded.
    if(!runnables.empty())
    {
        logger->addError("Module cannot be removed if there is a loaded application");
        return false;
    }

    Module* mod = knowledge.getModule(szModName);
    if(!mod)
        return false;

    return knowledge.removeModule(mod);
}

bool Manager::removeResource(const char* szResName)
{
    //Note: use it with care. it is better we first check that no application
    //is loaded.
    if(!runnables.empty())
    {
        logger->addError("Resource cannot be removed if there is a loaded application");
        return false;
    }

    GenericResource* res = knowledge.getResource(szResName);
    if(!res)
        return false;

    return knowledge.removeResource(res);
}



bool Manager::loadApplication(const char* szAppName)
{
    __CHECK_NULLPTR(szAppName);

    if(!allStopped())
    {
        logger->addError("Please stop current running application first.");
        return false;
    }

    strAppName = szAppName;

    // set all resources as unavailable
    ResourcePContainer allresources = knowledge.getResources();
    for(unsigned int i=0; i<allresources.size(); i++)
    {
        Computer* comp = dynamic_cast<Computer*>(allresources[i]);
        if(comp)
            comp->setAvailability(false);
    }

    return prepare(true);
}


bool Manager::saveApplication(const char* szAppName, const char* fileName)
{
    Application* pApp = knowledge.getApplication();
    __CHECK_NULLPTR(pApp);

    XmlAppSaver appsaver(fileName);
    return knowledge.saveApplication(&appsaver, pApp);
}


bool Manager::loadBalance(void)
{
    updateResources();
    bool ret = prepare(false);
    return ret;
}


bool Manager::prepare(bool silent)
{
    knowledge.reasolveDependency(strAppName.c_str(), bAutoDependancy, silent);

    clearExecutables();
    connections.clear();
    modules.clear();
    resources.clear();
    connections = knowledge.getSelConnection();
    modules = knowledge.getSelModules();
    resources = knowledge.getSelResources();

    /**
     *  we need to initialize a module with a local broker if the
     *  host property is set to "localhost".
     *
     * TODO: Resources should also be added to the relevant executable. up to now
     *  all of them will be handled by manager.
     */

    ModulePIterator itr;
    int id = 0;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
        
        Broker* broker = NULL;

        /*
        string strCurrentBroker;
        if(compareString((*itr)->getBroker(), BROKER_YARPRUN))
            strCurrentBroker = BROKER_YARPRUN;
        else
            strCurrentBroker = strDefBroker;

        if(compareString((*itr)->getHost(), "localhost"))
            broker = new LocalBroker;
        else if(strCurrentBroker == string(BROKER_YARPRUN))
            broker = new YarpBroker;
        //else if( for other brokers )
        //...
        */

        if(compareString((*itr)->getBroker(), BROKER_YARPDEV))
        {
            if(compareString((*itr)->getHost(), "localhost"))
               broker = new YarpdevLocalBroker();
            else
               broker = new YarpdevYarprunBroker(); 
        }
        else if(compareString((*itr)->getBroker(), BROKER_LOCAL))
        {
            if(compareString((*itr)->getHost(), "localhost"))
                broker = new LocalBroker();
        }    
        else if(compareString((*itr)->getBroker(), BROKER_YARPRUN))
                broker = new YarpBroker();

        /**
         * using default broker if it is still NULL
         */
        if(!broker)
        {
            if(compareString((*itr)->getHost(), "localhost"))
                broker = new LocalBroker;
            else
                broker = new YarpBroker;
            //OSTRINGSTREAM war;
            //war<<"Deployer "<<strCurrentBroker<<" does not exist! (using default (yarprun) deployer)";
            //logger->addWarning(war);
            //broker = new YarpBroker;
            //strCurrentBroker = BROKER_YARPRUN;
        }

        Executable* exe = new Executable(broker, (MEvent*)this, bWithWatchDog);
        exe->setID(id++);
        exe->setCommand((*itr)->getName());
        exe->setParam((*itr)->getParam());
        exe->setHost((*itr)->getHost());
        exe->setStdio((*itr)->getStdio());
        exe->setWorkDir((*itr)->getWorkDir());
        //if(strCurrentBroker == string(BROKER_YARPRUN))
       // {
            string env = string("YARP_PORT_PREFIX=") +
                            string((*itr)->getPrefix());
            exe->setEnv(env.c_str());
       // }

        /**
         * Adding connections to their owners
         * TODO: check whether this is still needed or not
         */

        //CnnIterator cnn;
        //for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
        //    if((*cnn).owner() == (*itr))
        //        exe->addConnection(*cnn);

        /**
         * Adding resources to their owners
         */
        for(unsigned int i=0; i<resources.size(); i++)
        {
            ResYarpPort* res = dynamic_cast<ResYarpPort*>(resources[i]);
            if(res && (res->owner() == (*itr)))
                exe->addResource(*res);
        }

        runnables.push_back(exe);
    }

    return true;
}

bool Manager::updateExecutable(unsigned int id, const char* szparam,
                const char* szhost, const char* szstdio,
                const char* szworkdir, const char* szenv )
{
    if(runnables.empty())
    {
        logger->addError("Application is not loaded.");
        return false;
    }

    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    Executable* exe = runnables[id];
    exe->setParam(szparam);
    exe->setHost(szhost);
    exe->setStdio(szstdio);
    exe->setWorkDir(szworkdir);
    exe->setEnv(szenv);
    return true;
}


bool Manager::updateConnection(unsigned int id, const char* from,
                                const char* to, const char* carrier)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    /*
    if(connections[id].owner())
    {
        OSTRINGSTREAM msg;
        msg<<"Connection ["<<connections[id].from()<<" -> ";
        msg<<connections[id].to()<<"] cannot be updated.";
        logger->addWarning(msg);
        return false;
    }
    */ 

    connections[id].setFrom(from);
    connections[id].setTo(to);
    connections[id].setCarrier(carrier);

    return true;
}


bool Manager::exist(unsigned int id)
{
    if(id>=resources.size())
    {
        logger->addError("Resource id is out of range.");
        return false;
    }

    GenericResource* res = resources[id];
    if(compareString(res->getName(), "localhost"))
        return true;

    if(dynamic_cast<Computer*>(res) || dynamic_cast<ResYarpPort*>(res))
    {
        if(res->getName())
        {
            //YarpBroker broker;
            //broker.init();
            string strPort = res->getName();
            if(strPort[0] != '/')
                strPort = string("/") + strPort;
            res->setAvailability(connector.exists(strPort.c_str()));
        }
    }
    return res->getAvailability();
}


bool Manager::updateResources(void)
{
    YarpBroker broker;
    broker.init();

    // finding all available yarp ports
    vector<string> ports;
    broker.getAllPorts(ports);

    ResourcePContainer allresources = knowledge.getResources();
    for(unsigned int i=0; i<allresources.size(); i++)
    {
        Computer* comp = dynamic_cast<Computer*>(allresources[i]);
        if(updateResource(comp))
        {
            //set all as unavailable
            for(int i=0; i<comp->peripheralCount(); i++)
            {
                ResYarpPort* res =
                    dynamic_cast<ResYarpPort*>(&comp->getPeripheralAt(i));
                if(res)
                    res->setAvailability(false);
            }

            // adding all available yarp ports as peripherals
            for(unsigned int i=0; i<ports.size(); i++)
            {
                ResYarpPort resport;
                resport.setName(ports[i].c_str());
                resport.setPort(ports[i].c_str());

                bool bfound = false;
                for(int i=0; i<comp->peripheralCount(); i++)
                {
                    ResYarpPort* res =
                        dynamic_cast<ResYarpPort*>(&comp->getPeripheralAt(i));
                    if(res && (string(res->getName()) == string(resport.getName())))
                    {
                        res->setAvailability(true);
                        bfound = true;
                        break;
                    }
                }
                if(!bfound)
                    comp->addPeripheral(resport);
            }
        }
    } // end of for

    return true;
}


bool Manager::updateResource(const char* szName)
{
    GenericResource* res = knowledge.getResource(szName);
    if(!res)
        return false;
    return updateResource(res);
}

bool Manager::updateResource(GenericResource* resource)
{
    YarpBroker broker;
    broker.init();

    Computer* comp = dynamic_cast<Computer*>(resource);
    if(!comp || !strlen(comp->getName()))
        return false;

    if(compareString(comp->getName(), "localhost"))
        return false;

    yarp::os::impl::SystemInfoSerializer info;
    string strServer = comp->getName();
    if(strServer[0] != '/')
        strServer = string("/") + strServer;
    if(!broker.getSystemInfo(strServer.c_str(), info))
    {
        logger->addError(broker.error());
        comp->setAvailability(false);
    }
    else
    {
        comp->setAvailability(true);

        comp->getMemory().setTotalSpace(info.memory.totalSpace*1024);
        comp->getMemory().setFreeSpace(info.memory.freeSpace*1024);

        comp->getStorage().setTotalSpace(info.storage.totalSpace*1024);
        comp->getStorage().setFreeSpace(info.storage.freeSpace*1024);

        comp->getNetwork().setIP4(info.network.ip4.c_str());
        comp->getNetwork().setIP6(info.network.ip6.c_str());
        comp->getNetwork().setMAC(info.network.mac.c_str());


        comp->getProcessor().setArchitecture(info.processor.architecture.c_str());
        comp->getProcessor().setCores(info.processor.cores);
        comp->getProcessor().setSiblings(info.processor.siblings);
        comp->getProcessor().setFrequency(info.processor.frequency);
        comp->getProcessor().setModel(info.processor.model.c_str());
        LoadAvg load;
        load.loadAverageInstant = (double)info.load.cpuLoadInstant;
        load.loadAverage1 = info.load.cpuLoad1;
        load.loadAverage5 = info.load.cpuLoad5;
        load.loadAverage15 = info.load.cpuLoad15;
        comp->getProcessor().setCPULoad(load);

        comp->getPlatform().setName(info.platform.name.c_str());
        comp->getPlatform().setDistribution(info.platform.distribution.c_str());
        comp->getPlatform().setRelease(info.platform.release.c_str());
    }
    return true;
}


bool Manager::existPortFrom(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    //YarpBroker connector;
    //connector.init();
    return connector.exists(connections[id].from());
}


bool Manager::existPortTo(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    //YarpBroker connector;
    //connector.init();
    return connector.exists(connections[id].to());
}


bool Manager::checkDependency(void)
{
    /**
     * checking for port resources availability
     * TODO:later it should change to use proper broker for resource cheking.
     *      up to now, we use only yraprun for checking port resources
     */
    bool ret = true;
    ResourcePIterator itrRes;
    for(itrRes=resources.begin(); itrRes!=resources.end(); itrRes++)
    {
        if(!(*itrRes)->getAvailability())
        {
            ret = false;
            OSTRINGSTREAM err;
            err<<"Resource "<<(*itrRes)->getName()<<" is not available!";
            logger->addError(err);
        }
    }

    return ret;
}



bool Manager::run(unsigned int id, bool async)
{
    if(runnables.empty())
    {
        logger->addError("Application is not loaded.");
        return false;
    }

    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    runnables[id]->disableAutoConnect();
    runnables[id]->start();

    if(async)
        return true;

    // waiting for running
    double base = yarp::os::Time::now();
    while(!timeout(base, RUN_TIMEOUT))
        if(running(id)) return true;

    OSTRINGSTREAM msg;
    msg<<"Failed to run "<<runnables[id]->getCommand();
    msg<<" on "<<runnables[id]->getHost();
    msg<<". (State: "<<runnables[id]->state();
    msg<<", paramete: "<<runnables[id]->getParam()<<")";
    logger->addError(msg);
    return false;
}

bool Manager::run(void)
{
    if(runnables.empty())
    {
        logger->addError("Application is not loaded.");
        return false;
    }

    if(!checkDependency())
    {
        if(bRestricted)
        {
            logger->addError("Some of external ports dependency are not satisfied.");
            return false;
        }
        else
            logger->addWarning("Some of external ports dependency are not satisfied.");
    }

    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {
        if(bAutoConnect)
            (*itr)->enableAutoConnect();
        else
            (*itr)->disableAutoConnect();
        (*itr)->start();
        yarp::os::Time::delay(0.2);
    }

    // waiting for running
    double base = yarp::os::Time::now();
    while(!timeout(base, RUN_TIMEOUT))
        if(allRunning()) break;

    if(!allRunning())
    {
        ExecutablePIterator itr;
        for(itr=runnables.begin(); itr!=runnables.end(); itr++)
            if((*itr)->state() != RUNNING)
            {
                OSTRINGSTREAM msg;
                msg<<"Failed to run "<<(*itr)->getCommand();
                msg<<" on "<<(*itr)->getHost();
                msg<<". (State: "<<(*itr)->state();
                msg<<", paramete: "<<(*itr)->getParam()<<")";
                logger->addError(msg);
            }

        if(bRestricted)
        {
            kill();
            return false;
        }
    }

    /* connecting extra ports*/
    if(bAutoConnect)
        if(!connectExtraPorts())
        {
            logger->addError("Failed to stablish some of connections.");
            if(bRestricted)
                return false;
        }

    return true;
}

bool Manager::stop(unsigned int id, bool async)
{
    if(runnables.empty())
    {
        logger->addError("Application is not loaded.");
        return false;
    }

    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    runnables[id]->stop();

    if(async)
        return true;

    // waiting for stop
    double base = yarp::os::Time::now();
    while(!timeout(base, STOP_TIMEOUT))
        if(!running(id)) return true;

    OSTRINGSTREAM msg;
    msg<<"Failed to stop "<<runnables[id]->getCommand();
    msg<<" on "<<runnables[id]->getHost();
    msg<<". (State: "<<runnables[id]->state();
    msg<<", paramete: "<<runnables[id]->getParam()<<")";
    logger->addError(msg);
    return false;
}


bool Manager::stop(void)
{
    if(runnables.empty())
        return true;

    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {        
        (*itr)->stop();
        yarp::os::Time::delay(0.2);
    }

    double base = yarp::os::Time::now();
    while(!timeout(base, STOP_TIMEOUT))
        if(allStopped()) break;

    if(!allStopped())
    {
        ExecutablePIterator itr;
        for(itr=runnables.begin(); itr!=runnables.end(); itr++)
            if( ((*itr)->state() != SUSPENDED) &&
                ((*itr)->state() != DEAD))
            {
                OSTRINGSTREAM msg;
                msg<<"Failed to stop "<<(*itr)->getCommand();
                msg<<" on "<<(*itr)->getHost();
                msg<<". (State: "<<(*itr)->state();
                msg<<", paramete: "<<(*itr)->getParam()<<")";
                logger->addError(msg);
            }
        return false;
    }

    return true;
}

bool Manager::kill(unsigned int id, bool async)
{
    if(runnables.empty())
    {
        logger->addError("Application is not loaded.");
        return false;
    }

    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    runnables[id]->kill();

    if(async)
        return true;

    double base = yarp::os::Time::now();
    while(!timeout(base, KILL_TIMEOUT))
        if(!running(id)) return true;

    OSTRINGSTREAM msg;
    msg<<"Failed to kill "<<runnables[id]->getCommand();
    msg<<" on "<<runnables[id]->getHost();
    msg<<". (State: "<<runnables[id]->state();
    msg<<", paramete: "<<runnables[id]->getParam()<<")";
    logger->addError(msg);
    return false;
}


bool Manager::kill(void)
{
    if(runnables.empty())
        return true;

    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {
        (*itr)->kill();
        yarp::os::Time::delay(0.2);
    }

    double base = yarp::os::Time::now();
    while(!timeout(base, KILL_TIMEOUT))
        if(allStopped()) break;

    if(!allStopped())
    {
        ExecutablePIterator itr;
        for(itr=runnables.begin(); itr!=runnables.end(); itr++)
            if( ((*itr)->state() != SUSPENDED) &&
                ((*itr)->state() != DEAD))
            {
                OSTRINGSTREAM msg;
                msg<<"Failed to kill "<<(*itr)->getCommand();
                msg<<" on "<<(*itr)->getHost();
                msg<<". (State: "<<(*itr)->state();
                msg<<", paramete: "<<(*itr)->getParam()<<")";
                logger->addError(msg);
            }
        return false;
    }

    return true;
}


void Manager::clearExecutables(void)
{
    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {
        // broker will be deleted by Executable
        delete (*itr);
    }
    runnables.clear();
}


bool Manager::connect(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    //YarpBroker connector;
    //connector.init();

    if( !connector.connect(connections[id].from(),
                            connections[id].to(),
                            connections[id].carrier(),
                            connections[id].isPersistent()) )
    {
        logger->addError(connector.error());
        //cout<<connector.error()<<endl;
        return false;
    }

    return true;
}

bool Manager::connect(void)
{
    //YarpBroker connector;
    //connector.init();
    CnnIterator cnn;
    for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
        if( !connector.connect((*cnn).from(), (*cnn).to(),
                               (*cnn).carrier(), (*cnn).isPersistent()) )
            {
                logger->addError(connector.error());
                //cout<<connector.error()<<endl;
                if(bRestricted)
                    return false;
            }
    return true;
}

bool Manager::disconnect(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    //YarpBroker connector;
    //connector.init();

    if( !connector.disconnect(connections[id].from(),
                            connections[id].to()) )
    {
        logger->addError(connector.error());
        //cout<<connector.error()<<endl;
        return false;
    }

    return true;
}

bool Manager::disconnect(void)
{
    //YarpBroker connector;
    //connector.init();
    CnnIterator cnn;
    for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
        if( !connector.disconnect((*cnn).from(), (*cnn).to()) )
            {
                logger->addError(connector.error());
                //cout<<connector.error()<<endl;
                return false;
            }
    return true;
}


bool Manager::rmconnect(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    if(!connector.rmconnect(connections[id].from(),
                            connections[id].to()) )
    {
        logger->addError(connector.error());
        return false;
    }

    return true;
}


bool Manager::rmconnect(void)
{
    CnnIterator cnn;
    for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
        if( !connector.rmconnect((*cnn).from(), (*cnn).to()) )
            {
                logger->addError(connector.error());
                return false;
            }
    return true;
}


bool Manager::connected(unsigned int id)
{
    if(id>=connections.size())
    {
        logger->addError("Connection id is out of range.");
        return false;
    }

    //YarpBroker connector;
    //connector.init();
    return connector.connected(connections[id].from(),
                            connections[id].to());
}


bool Manager::connected(void)
{
    //YarpBroker connector;
    //connector.init();
    CnnIterator cnn;
    bool bConnected = true;
    for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
        if( !connector.connected((*cnn).from(), (*cnn).to()) )
            bConnected = false;
    return bConnected;
}

bool Manager::checkPortsAvailable(Broker* broker)
{
    CnnIterator itr;
    for(itr=connections.begin(); itr!=connections.end(); itr++)
    {
       //if(!(*itr).owner() )
       // {
            if(!broker->exists((*itr).to()) ||
                !broker->exists((*itr).from()))
                    return false;
       // }
    }
    return true;
}


bool Manager::connectExtraPorts(void)
{
    //YarpBroker connector;
    //connector.init();

    double base = yarp::os::Time::now();
    while(!timeout(base, 10.0))
        if(checkPortsAvailable(&connector))
            break;

    CnnIterator cnn;
    for(cnn=connections.begin(); cnn!=connections.end(); cnn++)
    {
        //if(!(*cnn).owner() )
        //{
            if( !connector.connect((*cnn).from(), (*cnn).to(),
                                   (*cnn).carrier()) )
            {
                logger->addError(connector.error());
                //cout<<connector.error()<<endl;
                return false;
            }
       //}
    }
    return true;
}

bool Manager::running(unsigned int id)
{
    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    RSTATE st = runnables[id]->state();
    if((st == RUNNING) || (st == CONNECTING) || (st == DYING))
            return true;
    return false;
}


bool Manager::allRunning(void)
{
    if(!runnables.size())
        return false;
    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {
        RSTATE st = (*itr)->state();
        if((st == RUNNING) || (st == CONNECTING) || (st==DYING))
            return true;
    }
    return false;
}


bool Manager::suspended(unsigned int id)
{
    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }
    RSTATE st = runnables[id]->state();
    if((st == SUSPENDED) || (st == DEAD))
            return true;
    return false;
}


bool Manager::allStopped(void)
{
    if(!runnables.size())
        return true;
    ExecutablePIterator itr;
    for(itr=runnables.begin(); itr!=runnables.end(); itr++)
    {
        RSTATE st = (*itr)->state();
        if( (st != SUSPENDED) && (st != DEAD))
            return false;
    }
    return true;
}

bool Manager::attachStdout(unsigned int id)
{
    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    if(!runnables[id]->getBroker()->attachStdout())
    {
        OSTRINGSTREAM msg;
        msg<<"Cannot attach to stdout of "<<runnables[id]->getCommand();
        msg<<" on "<<runnables[id]->getHost();
        msg<<". (State: "<<runnables[id]->state();
        msg<<", paramete: "<<runnables[id]->getParam()<<") ";
        msg<<"because "<<runnables[id]->getBroker()->error();
        logger->addError(msg);
        return false;
    }
    return true;
}

bool Manager::detachStdout(unsigned int id)
{
    if(id>=runnables.size())
    {
        logger->addError("Module id is out of range.");
        return false;
    }

    runnables[id]->getBroker()->detachStdout();
    return true;
}

bool Manager::timeout(double base, double timeout)
{
    yarp::os::Time::delay(1.0);
    if((yarp::os::Time::now()-base) > timeout)
        return true;
    return false;
}


void Manager::onExecutableStart(void* which) {}
void Manager::onExecutableStop(void* which)  {}
void Manager::onCnnStablished(void* which) {}
void Manager::onExecutableDied(void* which) {}
void Manager::onExecutableFailed(void* which) {}
void Manager::onExecutableStdout(void* which, const char* msg) {}
void Manager::onCnnFailed(void* which) {}
void Manager::onError(void* which) {}


/*
bool Manager::loadModule(const char* szModule, const char* szHost)
{
    __CHECK_NULLPTR(szModule);
    strAppName = szModule;

    SingleAppLoader appLoader(szModule, szHost);
    if(!appLoader.init())
    {
        logger->addError("Error initializing SingleAppLoader.");
        return false;
    }

    if(!createKnowledgeBase(appLoader))
    {
        logger->addError("Cannot create knowledge base");
        return false;
    }

    return prepare();

}
*/


