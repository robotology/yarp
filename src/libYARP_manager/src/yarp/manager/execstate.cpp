/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/execstate.h>
#include <yarp/manager/executable.h>
#include <yarp/os/Time.h>

#include <iostream>

using namespace std;
using namespace FSM;
using namespace yarp::manager;


/**
 * Initializing event factory
 */

Event* EventFactory::startEvent = new Event("start");
Event* EventFactory::stopEvent = new Event("stop");
Event* EventFactory::killEvent = new Event("kill");
Event* EventFactory::failedEvent = new Event("failed");
Event* EventFactory::recoverEvent = new Event("recover");
Event* EventFactory::startModuleEventOk = new Event("startModule:ok");
Event* EventFactory::startModuleEventFailed = new Event("startModule:failed");
Event* EventFactory::stopModuleEventOk = new Event("stopModule:ok");
Event* EventFactory::stopModuleEventFailed = new Event("stopModule:failed");
Event* EventFactory::killModuleEventOk = new Event("killModule:ok");
Event* EventFactory::killModuleEventFailed = new Event("killModule:failed");
Event* EventFactory::connectAllPortsEventOk = new Event("connectAllPorts:ok");
Event* EventFactory::connectAllPortsEventFailed = new Event("connectAllPorts:failed");
Event* EventFactory::disconnectAllPortsEventOk = new Event("disconnectAllPorts:ok");



/**
 * Class Suspended
 */
Suspended::Suspended(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "SUSPENDED")
{
    executable = pExecutable;
}


Suspended::~Suspended() = default;

void Suspended::start()
{
    castEvent(EventFactory::startEvent);
}

void Suspended::stop()
{
    executable->getEvent()->onExecutableStop(executable);
}

void Suspended::kill()
{
    castEvent(EventFactory::killEvent);
}

void Suspended::moduleFailed() { /* do nothing*/ }


// refresh() from Suspended can be used for recovering from
// unexptected termination of manager.
void Suspended::refresh()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    int ret = executable->getBroker()->running();
    if(ret == 1)
    {
        executable->getEvent()->onExecutableStart(executable);
        castEvent(EventFactory::recoverEvent);
    } else if (ret == -1) {
        logger->addError(executable->getBroker()->error());
    }
}


/**
 * Class Ready
 */
Ready::Ready(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "READY")
{
    executable = pExecutable;
    bAborted = false;
}


Ready::~Ready() = default;

bool Ready::checkPriorityPorts()
{
    CnnIterator itr;
    for(itr=executable->getConnections().begin();
        itr!=executable->getConnections().end(); itr++)
    {
        if ((*itr).withPriority()
            && !executable->getBroker()->exists((*itr).from())) {
            return false;
        }
    }
    return true;
}

bool Ready::checkResources(bool silent)
{
    bool allOK = true;
    ResourceIterator itr;
    for(itr=executable->getResources().begin();
        itr!=executable->getResources().end(); itr++)
    {
        if(!executable->getBroker()->exists((*itr).getPort())) {
            allOK = false;
            if (silent) {
                return false;
            } else {
                OSTRINGSTREAM msg;
                msg<<(*itr).getPort()<<" does not exist";
                ErrorLogger::Instance()->addError(msg);
                continue;
            }
        }
        // check the rpc request/reply if required
        if(strlen((*itr).getRequest()) != 0) {
            const char* reply = executable->getBroker()->requestRpc((*itr).getPort(),
                                                                    (*itr).getRequest(),
                                                                    (*itr).getTimeout());
            if(reply == nullptr) {
                allOK = false;
                OSTRINGSTREAM msg;
                msg<<"cannot request resource "<<(*itr).getPort()<<" for "<<(*itr).getRequest();
                ErrorLogger::Instance()->addError(msg);
                if (silent) {
                    return false;
                } else {
                    continue;
                }
            }

            if(!compareString(reply, (*itr).getReply())) {
                allOK = false;
                OSTRINGSTREAM msg;
                msg<<"waiting for the expected reply from resource "<<(*itr).getPort();
                msg<<" for request "<<(*itr).getRequest();
                msg<<". (expected="<<(*itr).getReply()<<", received="<<reply<<")";
                ErrorLogger::Instance()->addWarning(msg);
                if (silent) {
                    return false;
                } else {
                    continue;
                }
            }
        }
    }
    return allOK;
}

bool Ready::timeout(double base, double timeout)
{
    yarp::os::SystemClock::delaySystem(1.0);
    if ((yarp::os::SystemClock::nowSystem() - base) > timeout) {
        return true;
    }
    return false;
}

void Ready::startModule()
{

    ErrorLogger* logger = ErrorLogger::Instance();

    // wait for priority ports if auto connecte is enabled
    if(executable->autoConnect())
    {
        bAborted = false;
        while(!checkPriorityPorts())
        {
            yarp::os::SystemClock::delaySystem(1.0);
            if (bAborted) {
                return;
            }
        }
    }

    // finding maximum resource-waiting timeout
    ResourceIterator itr;
    double maxTimeout = 0;
    for(itr=executable->getResources().begin();
        itr!=executable->getResources().end(); itr++)
    {
        if ((*itr).getTimeout() > maxTimeout) {
            maxTimeout = (*itr).getTimeout();
        }
    }

    // waiting for resources
    double base = yarp::os::SystemClock::nowSystem();
    while(!checkResources()) {
        if (bAborted) {
            return;
        }

        if(timeout(base, maxTimeout)) {
            // give it the last try and collect the error messages
            if(!checkResources(false)) {
                OSTRINGSTREAM msg;
                msg<<"cannot run "<<executable->getCommand()<<" on "<<executable->getHost();
                msg<<" : Timeout while waiting for "<<executable->getHost();
                logger->addError(msg);

                castEvent(EventFactory::startModuleEventFailed);
                executable->getEvent()->onExecutableDied(executable);
                return;
            }
        }
    }

    if (executable->getPostExecWait() > 0)
    {
        yarp::os::SystemClock::delaySystem(executable->getPostExecWait());
    }
    executable->restoreOriginalPostExecWait();
    if(!executable->getBroker()->start())
    {
        OSTRINGSTREAM msg;
        msg<<"cannot run "<<executable->getCommand()<<" on "<<executable->getHost();
        if (executable->getBroker()->error()) {
            msg << " : " << executable->getBroker()->error();
        }
        logger->addError(msg);

        castEvent(EventFactory::startModuleEventFailed);
        executable->getEvent()->onExecutableDied(executable);
    }
    else
    {
        castEvent(EventFactory::startModuleEventOk);
        executable->getEvent()->onExecutableStart(executable);
    }
}


void Ready::kill()
{
    bAborted = true;
    castEvent(EventFactory::killEvent);
}

void Ready::moduleFailed() { /* do nothing */ }


/**
 * Class Connecting
 */
Connecting::Connecting(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "CONNECTING"),
                      executable(pExecutable),
                      bAborted(false)
{}

Connecting::~Connecting() = default;

bool Connecting::checkNormalPorts()
{
    CnnIterator itr;
    for(itr=executable->getConnections().begin();
        itr!=executable->getConnections().end(); itr++)
    {
        if (!executable->getBroker()->exists((*itr).to()) || !executable->getBroker()->exists((*itr).from())) {
            return false;
        }
    }
    return true;
}


void Connecting::connectAllPorts()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    if(executable->autoConnect())
    {
        /**
         *  wait for required ports if auto connecte is enabled
         */
        bAborted = false;
        while(!checkNormalPorts())
        {
            yarp::os::SystemClock::delaySystem(1.0);
            if (bAborted) {
                return;
            }
        }

        CnnIterator itr;
        for(itr=executable->getConnections().begin();
            itr!=executable->getConnections().end(); itr++)
        {
            if( !executable->getBroker()->connect((*itr).from(), (*itr).to(),
                                                 (*itr).carrier()) )
            {
                OSTRINGSTREAM msg;
                msg<<"cannot connect "<<(*itr).from() <<" to "<<(*itr).to();
                if (executable->getBroker()->error()) {
                    msg << " : " << executable->getBroker()->error();
                }
                logger->addError(msg);
            } else {
                executable->getEvent()->onCnnStablished(&(*itr));
            }
        }
    }

    castEvent(EventFactory::connectAllPortsEventOk);
}

void Connecting::refresh()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    int ret = executable->getBroker()->running();
    if (ret == 0) {
        Connecting::moduleFailed();
    } else if (ret == -1) {
        logger->addError(executable->getBroker()->error());
    }
}

void Connecting::kill()
{
    bAborted = true;
    castEvent(EventFactory::killEvent);
}

void Connecting::moduleFailed()
{
    bAborted = true;
    castEvent(EventFactory::failedEvent);
    executable->getEvent()->onExecutableFailed(executable);
}


/**
 * Class Running
 */
Running::Running(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "RUNNING")
{
    executable = pExecutable;
}


Running::~Running() = default;

void Running::refresh()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    int ret = executable->getBroker()->running();
    if (ret == 0) {
        Running::moduleFailed();
    } else if (ret == -1) {
        logger->addError(executable->getBroker()->error());
    }
}

void Running::start()
{
    executable->getEvent()->onExecutableStart(executable);
}


void Running::stop()
{
    castEvent(EventFactory::stopEvent);
}

void Running::kill()
{
    castEvent(EventFactory::killEvent);
}

void Running::moduleFailed()
{
    castEvent(EventFactory::failedEvent);
    executable->getEvent()->onExecutableFailed(executable);
}

void Running::connectionFailed(void* which)
{
    executable->getEvent()->onCnnFailed(which);
}


/**
 * Class Dying
 */
Dying::Dying(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "DYING")
{
    executable = pExecutable;
}


Dying::~Dying() = default;

void Dying::stopModule()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    if (executable->getPostStopWait() > 0)
    {
        yarp::os::SystemClock::delaySystem(executable->getPostStopWait());
    }
    executable->restoreOriginalPostStopWait();
    if(!executable->getBroker()->stop())
    {
        OSTRINGSTREAM msg;
        msg<<"cannot stop "<<executable->getCommand()<<" on "<<executable->getHost();
        if (executable->getBroker()->error()) {
            msg << " : " << executable->getBroker()->error();
        }
        logger->addError(msg);
        executable->getEvent()->onError(executable);
        castEvent(EventFactory::stopModuleEventFailed);
    }
    else
    {
        castEvent(EventFactory::stopModuleEventOk);
        executable->getEvent()->onExecutableStop(executable);
    }
}

void Dying::killModule()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    if(!executable->getBroker()->kill())
    {
        OSTRINGSTREAM msg;
        msg<<"cannot kill "<<executable->getCommand()<<" on "<<executable->getHost();
        if (executable->getBroker()->error()) {
            msg << " : " << executable->getBroker()->error();
        }
        logger->addError(msg);
        executable->getEvent()->onError(executable);
        castEvent(EventFactory::killModuleEventFailed);
    }
    else
    {
        castEvent(EventFactory::killModuleEventOk);
        executable->getEvent()->onExecutableDied(executable);
    }
}


void Dying::disconnectAllPorts()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    if(executable->autoConnect())
    {
        CnnIterator itr;
        for(itr=executable->getConnections().begin();
            itr!=executable->getConnections().end(); itr++)
        {
            if( !executable->getBroker()->disconnect((*itr).from(), (*itr).to(), (*itr).carrier()) )
            {
                OSTRINGSTREAM msg;
                msg<<"cannot disconnect "<<(*itr).from() <<" to "<<(*itr).to();
                if (executable->getBroker()->error()) {
                    msg << " : " << executable->getBroker()->error();
                }
                logger->addError(msg);
            } else {
                executable->getEvent()->onCnnReleased(&(*itr));
            }
        }
    }
    // We do not need to handle event disconnectAllPortsEventOk
}

void Dying::refresh()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    int ret = executable->getBroker()->running();
    if (ret == 0) {
        Dying::moduleFailed();
    } else if (ret == -1) {
        logger->addError(executable->getBroker()->error());
    }
}

void Dying::kill() { /* do nothing */ }

void Dying::moduleFailed()
{
    // Notice that we should not call onExecutableFailed
    // in DYING state!
    castEvent(EventFactory::failedEvent);
    executable->getEvent()->onExecutableDied(executable);
}




/**
 * Class Dead
 */
Dead::Dead(Executable* pExecutable, FSM::IEventSink* pEventSink)
                    : StateBase(pEventSink, "DEAD")
{
    executable = pExecutable;
}


Dead::~Dead() = default;

void Dead::start()
{
    castEvent(EventFactory::startEvent);
}

void Dead::stop()
{
    executable->getEvent()->onExecutableStop(executable);
}


void Dead::kill()
{
    castEvent(EventFactory::killEvent);
}

// refresh() from Dead can be used for recovering from
// unexpect termination of manager.
void Dead::refresh()
{
    ErrorLogger* logger = ErrorLogger::Instance();
    int ret = executable->getBroker()->running();
    if(ret == 1)
    {
        executable->getEvent()->onExecutableStart(executable);
        castEvent(EventFactory::recoverEvent);
    } else if (ret == -1) {
        logger->addError(executable->getBroker()->error());
    }
}


void Dead::moduleFailed() { /* do nothing*/ }



/**
 * Class ExecMachine
 */
ExecMachine::ExecMachine(Executable* pExecutable)
{
    executable = pExecutable;
    // creating states
    suspended = new Suspended(executable, this);
    ready = new Ready(executable, this);
    connecting = new Connecting(executable, this);
    running = new Running(executable, this);
    dying = new Dying(executable, this);
    dead = new Dead(executable, this);

    // setting initial state
    setInitState(suspended);

    // transitions from suspended
    addTransition(suspended, EventFactory::startEvent, ready);
    addTransition(suspended, EventFactory::recoverEvent, running);  //recovering
    addTransition(suspended, EventFactory::killEvent, dying);
    addTransition(suspended, EventFactory::killModuleEventOk, dead);
    addTransition(suspended, EventFactory::failedEvent, suspended);

    // transitions from ready
    addTransition(ready, EventFactory::startModuleEventOk, connecting);
    addTransition(ready, EventFactory::startModuleEventFailed, dead);
    addTransition(ready, EventFactory::killEvent, dying);
    addTransition(ready, EventFactory::killModuleEventOk, dead);

    // transitions from connecting
    addTransition(connecting, EventFactory::connectAllPortsEventOk, running);
    addTransition(connecting, EventFactory::failedEvent, dead);
    addTransition(connecting, EventFactory::killEvent, dying);
    addTransition(connecting, EventFactory::killModuleEventOk, dead);

    // transitions from running
    addTransition(running, EventFactory::stopEvent, dying);
    addTransition(running, EventFactory::failedEvent, dead);
    addTransition(running, EventFactory::killEvent, dying);
    addTransition(running, EventFactory::killModuleEventOk, dead);

    // transitions from dying
    addTransition(dying, EventFactory::stopModuleEventOk, suspended);
    addTransition(dying, EventFactory::stopModuleEventFailed, dying);
    addTransition(dying, EventFactory::killModuleEventOk, dead);
    addTransition(dying, EventFactory::killModuleEventFailed, dying);
    addTransition(dying, EventFactory::failedEvent, dead);
    addTransition(dying, EventFactory::startModuleEventOk, connecting);
    addTransition(dying, EventFactory::startModuleEventFailed, dead);

    // transitions from dead
    addTransition(dead, EventFactory::startEvent, ready);
    addTransition(dead, EventFactory::killEvent, dying);
    addTransition(dead, EventFactory::recoverEvent, running);   // recovering
    addTransition(dead, EventFactory::killModuleEventOk, dead);
    addTransition(dead, EventFactory::killModuleEventFailed, dead);
    addTransition(dead, EventFactory::stopModuleEventOk, dead);
    addTransition(dead, EventFactory::stopModuleEventFailed, dead);
    addTransition(dead, EventFactory::startModuleEventOk, connecting);
    addTransition(dead, EventFactory::startModuleEventFailed, dead);
    addTransition(dead, EventFactory::failedEvent, dead);

}

ExecMachine::~ExecMachine()
{
    delete running;
    delete suspended;
    delete ready;
    delete connecting;
    delete dying;
    delete dead;
}

void ExecMachine::refresh()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->refresh();
    }
}

void ExecMachine::start()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->start();
    }
}

void ExecMachine::stop()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->stop();
    }
}

void ExecMachine::kill()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->kill();
    }
}

void ExecMachine::startModule()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->startModule();
    }
}

void ExecMachine::stopModule()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->stopModule();
    }
}

void ExecMachine::killModule()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->killModule();
    }
}

void ExecMachine::connectAllPorts()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->connectAllPorts();
    }
}

void ExecMachine::disconnectAllPorts()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->disconnectAllPorts();
    }
}

void ExecMachine::moduleFailed()
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->moduleFailed();
    }
}

void ExecMachine::connectionFailed(void* which)
{
    auto* tr = dynamic_cast<ITransition*>(currentState());
    if (tr) {
        tr->connectionFailed(which);
    }
}

// For debugging
void ExecMachine::onTransition(StateBase* previous,
                                Event* event, StateBase* current)
{
    /*
    std::cout<<executable->getID()<<": ";
    std::cout<<"["<<previous->getName()<<"] ";
    std::cout<<"--- ("<<event->getName()<<"/"<<event->getTimeStamp()<<") --> ";
    std::cout<<"["<<current->getName()<<"]"<<endl;
    */
}
