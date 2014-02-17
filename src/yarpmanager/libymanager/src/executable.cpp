/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/executable.h>

using namespace yarp::manager;

Executable::Executable(Broker* _broker, MEvent* _event,
                    bool _bWatchDog)
{
    bAutoConnect = true;
    broker = _broker;
    event = _event;
    bWatchDog = _bWatchDog;
    logger  = ErrorLogger::Instance();
    broker->setEventSink(dynamic_cast<BrokerEventSink*>(this));
    execMachine = new ExecMachine(this);
    startWrapper = new ConcurentWrapper(this, &Executable::startImplement);
    stopWrapper = new ConcurentWrapper(this, &Executable::stopImplement);
    killWrapper = new ConcurentWrapper(this, &Executable::killImplement);


    if(bWatchDog)
        watchdogWrapper = new ConcurentRateWrapper(this, &Executable::watchdogImplement);
    else
        watchdogWrapper = NULL;
}

Executable::~Executable()
{
    if(watchdogWrapper)
        delete watchdogWrapper;
    delete startWrapper;
    delete stopWrapper;
    delete killWrapper;
    delete execMachine;
    if(broker)
        delete broker;
}


bool Executable::initialize(void)
{
    __CHECK_NULLPTR(broker);
    __CHECK_NULLPTR(event);

    semInitialize.wait();
    bool ret = broker->init(strCommand.c_str(),
                        strParam.c_str(),
                        strHost.c_str(),
                        strStdio.c_str(),
                        strWorkdir.c_str(),
                        strEnv.c_str());
    semInitialize.post();
    if(!ret)
    {
        OSTRINGSTREAM msg;
        msg<<"cannot initialize broker. : ";
        if(broker->error())
            msg<<broker->error();
        logger->addError(msg);
        event->onExecutableDied(this);
        return false;
    }
    return true;
}


bool Executable::start(void)
{

    if(!initialize())
    {
      event->onExecutableDied(this);
      return false;
    }

    if(!startWrapper->isRunning())
    {
        startWrapper->start();
        if(watchdogWrapper && !watchdogWrapper->isRunning())
            watchdogWrapper->start();
        return true;
    }
    return false;
}


void Executable::startImplement()
{
    execMachine->start();
    execMachine->startModule();
    execMachine->connectAllPorts();
}


void Executable::stop(void)
{
    if(!broker->initialized())
        initialize();
    if(!stopWrapper->isRunning())
    {
        if(watchdogWrapper && watchdogWrapper->isRunning())
            watchdogWrapper->stop();
        stopWrapper->start();
    }
}

void Executable::stopImplement(void)
{
    execMachine->stop();
    execMachine->disconnectAllPorts();
    execMachine->refresh();
    execMachine->stopModule();
}


void Executable::kill(void)
{
    if(!broker->initialized())
        initialize();

    // Notice that kill can be called from multiple threads
    if(watchdogWrapper && watchdogWrapper->isRunning())
        watchdogWrapper->stop();
    killWrapper->start();
}

void Executable::killImplement(void)
{
    execMachine->kill();
    execMachine->killModule();
}


RSTATE Executable::state(void)
{

    if(!broker->initialized())
        initialize();

    // Updating real module state on demand
    // Notice that this is a blocking method
    execMachine->refresh();

    const char* strState = execMachine->currentState()->getName();

    if(compareString(strState, "SUSPENDED"))
        return SUSPENDED;
    if(compareString(strState, "READY"))
        return READY;
    if(compareString(strState, "CONNECTING"))
        return CONNECTING;
    if(compareString(strState, "RUNNING"))
        return RUNNING;
    if(compareString(strState, "DEAD"))
        return DEAD;
    if(compareString(strState, "DYING"))
        return DYING;

    std::cerr<<"Unknown state!"<<endl;
    return STUNKNOWN;
}

void Executable::onBrokerStdout(const char* msg)
{
    event->onExecutableStdout(this, msg);
}


void Executable::watchdogImplement(void)
{

    if(!broker->running())
            execMachine->moduleFailed();

    if(bAutoConnect)
    {
        CnnIterator itr;
        for(itr=connections.begin(); itr!=connections.end(); itr++)
            if( !broker->connected((*itr).from(), (*itr).to()) )
                execMachine->connectionFailed(&(*itr));
    }
}
