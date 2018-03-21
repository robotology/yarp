/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_FSM
#define YARP_MANAGER_FSM

#include <iostream>
#include <string>
#include <typeinfo>
#include <exception>
#include <map>

#include <yarp/os/Semaphore.h>

namespace FSM {
    class Event;
    class IEventSink;
    class StateBase;
    class StateMachineBase;
}

/**
 * class IEventSink
 */

class FSM::Event
{
public:
    Event(const char* szName) {
        if(szName) strName = szName;
        timeStamp = 0;
    }
    virtual ~Event() {}

    void setTimeStamp(double t) { timeStamp = t; }
    double getTimeStamp(void) { return timeStamp; }
    const char* getName(void) { return strName.c_str(); }

    inline bool operator==(const Event& alt) const {
        return ((strName == alt.strName)); }

private:
    std::string strName;
    double timeStamp;
};



/**
 * class IEventSink
 */

class FSM::IEventSink
{
public:

    IEventSink() {}
    virtual ~IEventSink() {}

   /**
   * @param event the event
   */
  virtual void castEvent (Event* event) = 0;

private:


};


/**
 * Class StateBase
 */
class FSM::StateBase
{
public:
    StateBase(IEventSink* pEventSink, const char* szName=NULL) {
        eventSink = pEventSink;
        if(szName) strName = szName;
    }

    virtual ~StateBase() {}

    const char* getName(void) {
        return strName.c_str(); }

protected:
    void castEvent(Event* event) {
        eventSink->castEvent(event);
    }

private:
    IEventSink* eventSink;
    std::string strName;
};

typedef std::map<FSM::StateBase*, std::map<FSM::Event*, FSM::StateBase*> > MyStateMap;
typedef std::map<FSM::StateBase*, std::map<FSM::Event*, FSM::StateBase*> >::iterator MyStateItr;


/**
 * Class StateMachineBase
 */
class FSM::StateMachineBase : public IEventSink
{
public:
    StateMachineBase() {
        state = NULL;
        currentTimeStamp = 0.0;
    }
    virtual ~StateMachineBase() {}

    StateBase* currentState(void) {
        try
        {
    //        typeid(*state);
            return state;
        }
        catch (std::exception& )
        {
            std::cerr<<"Exception in currentState(): Initial state is not set!"<<std::endl;
            std::terminate();
        }
    }

    void setInitState(StateBase* pState) {
        if(!state) state = pState;
    }

    void addTransition(StateBase* source, Event* event, StateBase* target) {
        try
        {
//             typeid(*target);
            transitions[source][event] = target;
        }
        catch (std::exception& typevar)
        {
            std::cerr<<"Exception in addTransition(): "<<typevar.what()<<std::endl;
            std::terminate();
        }
    }

protected:

    /**
     * Callback onTransition represents the change in the states
     */
    virtual void onTransition(StateBase* previous, Event* event, StateBase* current) {}
    virtual void onEventMissed(StateBase* state, Event* event) {}

public: // implementing IEventSink::castEvent()

    virtual void castEvent(Event* event) override
    {
        semEvent.wait();
        if(!state)
        {
            std::cerr<<"Initial state is not set!"<<std::endl;
            semEvent.post();
            return;
        }

        if(event->getTimeStamp() < currentTimeStamp )
        {
            onEventMissed(state, event);
            semEvent.post();
            return;
        }

        currentTimeStamp = event->getTimeStamp();

        MyStateItr it;
        it = transitions.find(state);
        if(it==transitions.end())
        {
            std::cerr<<"No transition is registered from state "<<state->getName()<<std::endl;
            semEvent.post();
            return;
        }

        std::map<Event*, StateBase*> row = transitions[state];
        std::map<Event*, StateBase*>::iterator itr2 = row.find(event);
        if(itr2 == row.end())
        {
            std::cerr<<"No transition is registered for event ";
            std::cerr<<event->getName()<<" ("<<event->getTimeStamp()<<")";
            std::cerr<<" from state "<<state->getName()<<std::endl;
            semEvent.post();
            return;
        }

        StateBase* previous = state;
        state = row[event];

        // calling callback
        onTransition(previous, event, state);
        semEvent.post();
    }

private:
    StateBase* state;
    MyStateMap transitions;
    yarp::os::Semaphore semEvent;
    double currentTimeStamp;

};


#endif // __YARP_MANAGER_FSM__
