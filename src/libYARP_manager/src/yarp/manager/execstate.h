/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_Exec_State
#define YARP_MANAGER_Exec_State

#include <yarp/manager/fsm.h>
#include <yarp/conf/api.h>


namespace yarp {
namespace manager {

class Executable;


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
    void start(void) override;
    void stop(void) override;
    void kill(void) override;
    void refresh(void) override;
    void moduleFailed(void) override;

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
    void kill(void) override;
    void startModule(void) override;
    void moduleFailed(void) override;

private:
    Executable* executable;
    bool bAborted;
    bool checkPriorityPorts(void);
    bool checkResources(bool silent=true);
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
    void refresh(void) override;
    void kill(void) override;
    void connectAllPorts(void) override;
    void moduleFailed(void) override;

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
    void refresh(void) override;
    void kill(void) override;
    void start(void) override;
    void stop(void) override;
    void moduleFailed(void) override;
    void connectionFailed(void* which) override;

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
    void refresh(void) override;
    void kill(void) override;
    void moduleFailed(void) override;
    void stopModule(void) override;
    void killModule(void) override;
    void disconnectAllPorts(void) override;

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
    void start(void) override;
    void stop(void) override;
    void kill(void) override;
    void refresh(void) override;
    void moduleFailed(void) override;

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
                    FSM::Event* event, FSM::StateBase* current) override;

private:
    Executable* executable;
    Running* running;
    Suspended* suspended;
    Ready* ready;
    Connecting* connecting;
    Dead* dead;
    Dying* dying;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_Exec_State__
