/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/executable.h>
#include <yarp/manager/yarpbroker.h>

using namespace yarp::manager;

Executable::Executable(Broker* _broker, MEvent* _event, Module *module,
                    bool _bWatchDog)
{
    bAutoConnect = true;
    broker = _broker;
    event = _event;
    bWatchDog = _bWatchDog;
    waitStart = 0.0;
    waitStop = 0.0;
    originalWaitStart = 0.0;
    originalWaitStop  = 0.0;
    Executable::module = module;
    logger  = ErrorLogger::Instance();
    broker->setEventSink(dynamic_cast<BrokerEventSink*>(this));
    execMachine = new ExecMachine(this);
    startWrapper = new ConcurentWrapper(this, &Executable::startImplement);
    stopWrapper = new ConcurentWrapper(this, &Executable::stopImplement);
    killWrapper = new ConcurentWrapper(this, &Executable::killImplement);
    theID = -1;

    if (bWatchDog) {
        watchdogWrapper = new ConcurentRateWrapper(this, &Executable::watchdogImplement);
    } else {
        watchdogWrapper = nullptr;
    }
}

Executable::~Executable()
{
    if (watchdogWrapper) {
        delete watchdogWrapper;
    }
    delete startWrapper;
    delete stopWrapper;
    delete killWrapper;
    delete execMachine;
    removeBroker();
}


bool Executable::initialize()
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
        if (broker->error()) {
            msg << broker->error();
        }
        logger->addError(msg);
        event->onExecutableDied(this);
        return false;
    }
    return true;
}


bool Executable::start()
{
    if(!initialize()) {
      event->onExecutableDied(this);
      return false;
    }

    if(!startWrapper->isRunning()) {
        startWrapper->start();
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


void Executable::stop()
{
    if (!broker->initialized()) {
        initialize();
    }

    if(!stopWrapper->isRunning()) {
        stopWatchDog();
        stopWrapper->start();
    }
}

void Executable::stopImplement()
{
    execMachine->stop();
    execMachine->disconnectAllPorts();
    execMachine->refresh();
    execMachine->stopModule();
}


void Executable::kill()
{
    if (!broker->initialized()) {
        initialize();
    }

    // Notice that kill can be called from multiple threads
    stopWatchDog();
    killWrapper->start();
}

void Executable::killImplement()
{
    execMachine->kill();
    execMachine->killModule();
}


RSTATE Executable::state()
{

    if (!broker->initialized()) {
        initialize();
    }

    // Updating real module state on demand
    // Notice that this is a blocking method
    execMachine->refresh();

    const char* strState = execMachine->currentState()->getName();

    if (compareString(strState, "SUSPENDED")) {
        return SUSPENDED;
    }
    if (compareString(strState, "READY")) {
        return READY;
    }
    if (compareString(strState, "CONNECTING")) {
        return CONNECTING;
    }
    if (compareString(strState, "RUNNING")) {
        return RUNNING;
    }
    if (compareString(strState, "DEAD")) {
        return DEAD;
    }
    if (compareString(strState, "DYING")) {
        return DYING;
    }

    std::cerr<<"Unknown state!"<<std::endl;
    return STUNKNOWN;
}

BrokerType Executable::getBrokerType()
{
    if (broker == nullptr)
    {
        return BrokerType::invalid;
    }
    else if (dynamic_cast<YarpBroker*>(broker))
    {
        return BrokerType::yarp;
    }
    else
    {
        return BrokerType::local;
    }

}

bool Executable::shouldChangeBroker()
{
    if (getBrokerType() == BrokerType::local &&
        strHost != "localhost")
    {
       return true;
    }
    else if (getBrokerType() == BrokerType::yarp &&
             strHost == "localhost")
    {
        return true;
    }
    return false;

}

void Executable::setAndInitializeBroker(Broker *_broker)
{
    if (_broker)
    {
        broker = _broker;
        initialize();
    }
}

bool Executable::startWatchDog() {
    if (watchdogWrapper == nullptr) {
        return false;
    }
    if (!watchdogWrapper->isRunning()) {
        watchdogWrapper->start();
    }
    return true;
}

void Executable::stopWatchDog() {
    if (watchdogWrapper && watchdogWrapper->isRunning()) {
        watchdogWrapper->stop();
    }
}

void Executable::onBrokerStdout(const char* msg)
{
    event->onExecutableStdout(this, msg);
}


void Executable::watchdogImplement()
{
    if (!broker->running()) {
        execMachine->moduleFailed();
    }

    if(bAutoConnect)
    {
        CnnIterator itr;
        for (itr = connections.begin(); itr != connections.end(); itr++) {
            if (!broker->connected((*itr).from(), (*itr).to(), (*itr).carrier())) {
                execMachine->connectionFailed(&(*itr));
            }
        }
    }
}
