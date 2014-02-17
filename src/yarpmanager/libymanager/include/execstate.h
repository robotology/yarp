// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __Exec_State__
#define __Exec_State__

#include "fsm.h"

class Executable;

//namespace ymm {


/**
 * all transitions are used in state machine
 */
class ITransition
{
public:
    virtual ~ITransition() {}
    virtual void kill(void) = 0;
    virtual void moduleFailed(void) = 0;
    virtual void refresh(void) {}
    virtual void connectionFailed(void* which) {}
    virtual void start(void) {}
    virtual void stop(void) {}
    virtual void startModule(void) {}
    virtual void stopModule(void) {}
    virtual void killModule(void) {}
    virtual void connectAllPorts(void) {}
    virtual void disconnectAllPorts(void) {}
};


class EventFactory
{
public:
    static FSM::Event* startEvent;
    static FSM::Event* stopEvent;
    static FSM::Event* killEvent;
    static FSM::Event* failedEvent;
    static FSM::Event* recoverEvent;
    static FSM::Event* startModuleEventOk;
    static FSM::Event* startModuleEventFailed;
    static FSM::Event* stopModuleEventOk;
    static FSM::Event* stopModuleEventFailed;
    static FSM::Event* killModuleEventOk;
    static FSM::Event* killModuleEventFailed;
    static FSM::Event* connectAllPortsEventOk;
    static FSM::Event* connectAllPortsEventFailed;
    static FSM::Event* disconnectAllPortsEventOk;
};


/**
 * class Suspended
 */
class Suspended : public ITransition, public FSM::StateBase
{
public:
    Suspended(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Suspended();

public:
    void start(void);
    void stop(void);
    void kill(void);
    void refresh(void);
    void moduleFailed(void);

public:
    Executable* executable;
};


/**
 * class Ready
 */
class Ready : public ITransition, public FSM::StateBase
{
public:
    Ready(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Ready();

public:
    void kill(void);
    void startModule(void);
    void moduleFailed(void);

private:
    Executable* executable;
    bool bAborted;
    bool checkPriorityPorts(void);
    bool checkResources(void);
    bool timeout(double base, double timeout);
};



/**
 * class Connecting
 */
class Connecting : public ITransition, public FSM::StateBase
{
public:
    Connecting(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Connecting();

public:
    void refresh(void);
    void kill(void);
    void connectAllPorts(void);
    void moduleFailed(void);

private:
    Executable* executable;
    bool checkNormalPorts(void);
    bool bAborted;
};



/**
 * class Running
 */
class Running : public ITransition, public FSM::StateBase
{
public:
    Running(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Running();

public:
    void refresh(void);
    void kill(void);
    void start(void);
    void stop(void);
    void moduleFailed(void);
    void connectionFailed(void* which);

private:
    Executable* executable;
};


/**
 * class Dying
 */
class Dying : public ITransition, public FSM::StateBase
{
public:
    Dying(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Dying();

public:
    void refresh(void);
    void kill(void);
    void moduleFailed(void);
    void stopModule(void);
    void killModule(void);
    void disconnectAllPorts(void);

private:
    Executable* executable;
};


/**
 * class Dead
 */
class Dead : public ITransition, public FSM::StateBase
{
public:
    Dead(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Dead();

public:
    void start(void);
    void stop(void);
    void kill(void);
    void refresh(void);
    void moduleFailed(void);

private:
    Executable* executable;
};


/**
 * Class ExecMachine
 */
class ExecMachine : public FSM::StateMachineBase
{
public:
    ExecMachine(Executable* pExecutable);
    ~ExecMachine();

    void refresh(void);
    void start(void);
    void stop(void);
    void kill(void);
    void startModule(void);
    void stopModule(void);
    void killModule(void);
    void connectAllPorts(void);
    void disconnectAllPorts(void);
    void moduleFailed(void);
    void connectionFailed(void* which);

protected:
    void onTransition(FSM::StateBase* previous,
                    FSM::Event* event, FSM::StateBase* current);

private:
    Executable* executable;
    Running* running;
    Suspended* suspended;
    Ready* ready;
    Connecting* connecting;
    Dead* dead;
    Dying* dying;
};



//}

#endif //__Exec_State__
