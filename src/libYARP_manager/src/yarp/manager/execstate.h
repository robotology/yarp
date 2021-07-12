/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    virtual ~ITransition() = default;
    virtual void kill() = 0;
    virtual void moduleFailed() = 0;
    virtual void refresh() {}
    virtual void connectionFailed(void* which) {}
    virtual void start() {}
    virtual void stop() {}
    virtual void startModule() {}
    virtual void stopModule() {}
    virtual void killModule() {}
    virtual void connectAllPorts() {}
    virtual void disconnectAllPorts() {}
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
    ~Suspended() override;

public:
    void start() override;
    void stop() override;
    void kill() override;
    void refresh() override;
    void moduleFailed() override;

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
    ~Ready() override;

public:
    void kill() override;
    void startModule() override;
    void moduleFailed() override;

private:
    Executable* executable;
    bool bAborted;
    bool checkPriorityPorts();
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
    ~Connecting() override;

public:
    void refresh() override;
    void kill() override;
    void connectAllPorts() override;
    void moduleFailed() override;

private:
    Executable* executable;
    bool checkNormalPorts();
    bool bAborted;
};



/**
 * class Running
 */
class Running : public ITransition, public FSM::StateBase
{
public:
    Running(Executable* pExecutable, FSM::IEventSink* pEventSink);
    ~Running() override;

public:
    void refresh() override;
    void kill() override;
    void start() override;
    void stop() override;
    void moduleFailed() override;
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
    ~Dying() override;

public:
    void refresh() override;
    void kill() override;
    void moduleFailed() override;
    void stopModule() override;
    void killModule() override;
    void disconnectAllPorts() override;

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
    ~Dead() override;

public:
    void start() override;
    void stop() override;
    void kill() override;
    void refresh() override;
    void moduleFailed() override;

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
    ~ExecMachine() override;

    void refresh();
    void start();
    void stop();
    void kill();
    void startModule();
    void stopModule();
    void killModule();
    void connectAllPorts();
    void disconnectAllPorts();
    void moduleFailed();
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
