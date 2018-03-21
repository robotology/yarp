/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_NAMESERVICEONTRIPLES_H
#define YARP_SERVERSQL_IMPL_NAMESERVICEONTRIPLES_H

#include <yarp/name/NameService.h>
#include <yarp/serversql/impl/TripleSource.h>
#include <yarp/serversql/impl/Allocator.h>
#include <yarp/serversql/impl/Subscriber.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/Semaphore.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 *
 * State information for a single name server operation on a database.
 *
 */
class NameTripleState
{
public:
    yarp::os::Bottle& cmd;
    yarp::os::Bottle& reply;
    yarp::os::Bottle& event;
    const yarp::os::Contact& remote;
    TripleSource& mem;
    bool bottleMode;
    bool nestedMode;

    NameTripleState(yarp::os::Bottle& cmd,
                    yarp::os::Bottle& reply,
                    yarp::os::Bottle& event,
                    const yarp::os::Contact& remote,
                    TripleSource& mem) :
            cmd(cmd),
            reply(reply),
            event(event),
            remote(remote),
            mem(mem),
            bottleMode(false),
            nestedMode(false)
    {
    }
};

/**
 *
 * An implementation of name service operators on a triple store.
 *
 */
class NameServiceOnTriples : public yarp::name::NameService
{
private:
    TripleSource *db;
    Allocator *alloc;
    Subscriber *subscriber;
    std::string lastRegister;
    yarp::os::Contact serverContact;
    yarp::os::Semaphore mutex;
    yarp::os::Semaphore access;
    bool gonePublic;
    bool silent;
    yarp::os::NameSpace *delegate;
public:
    NameServiceOnTriples() :
            db(nullptr),
            alloc(nullptr),
            subscriber(nullptr),
            lastRegister(""),
            mutex(1),
            access(1),
            gonePublic(false),
            silent(false),
            delegate(nullptr)
    {
    }

    void open(TripleSource *db,
              Allocator *alloc,
              const yarp::os::Contact& serverContact)
    {
        this->db = db;
        this->alloc = alloc;
        this->serverContact = serverContact;
    }

    void setSubscriber(Subscriber *subscriber)
    {
        this->subscriber = subscriber;
    }

    void setSilent(bool flag)
    {
        this->silent = flag;
    }

    yarp::os::Contact query(const yarp::os::ConstString& portName,
                            NameTripleState& act,
                            const yarp::os::ConstString& prefix,
                            bool nested = false);

    virtual bool announce(const yarp::os::ConstString& name, int activity) override;

    virtual yarp::os::Contact query(const yarp::os::ConstString& portName) override;

    bool cmdQuery(NameTripleState& act, bool nested = false);

    bool cmdRegister(NameTripleState& act);

    bool cmdUnregister(NameTripleState& act);

    bool cmdList(NameTripleState& act);

    bool cmdSet(NameTripleState& act);

    bool cmdGet(NameTripleState& act);

    bool cmdCheck(NameTripleState& act);

    bool cmdRoute(NameTripleState& act);

    bool cmdGc(NameTripleState& act);

    bool cmdHelp(NameTripleState& act);

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) override;

    virtual void goPublic() override
    {
        gonePublic = true;
    }

    void lock() override;

    void unlock() override;

    void setDelegate(yarp::os::NameSpace *delegate)
    {
        this->delegate = delegate;
    }

    yarp::os::NameSpace *getDelegate()
    {
        return delegate;
    }
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_NAMESERVICEONTRIPLES_H
