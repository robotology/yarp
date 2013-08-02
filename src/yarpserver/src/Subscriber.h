// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_SUBSCRIBER_INC
#define YARPDB_SUBSCRIBER_INC

#include <yarp/name/NameService.h>
#include "ConnectThread.h"

#include <yarp/os/ConstString.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/NameStore.h>

/**
 *
 * Abstract interface for maintaining persistent connections.
 *
 */
class Subscriber : public yarp::name::NameService {
private:
    yarp::os::NameStore *store;
    ConnectManager manager;
public:
    Subscriber() : store(0/*NULL*/) {}

    void setStore(yarp::os::NameStore& store) { this->store = &store; }
    yarp::os::NameStore *getStore() { return store; }

    void connect(const yarp::os::ConstString& src,
                 const yarp::os::ConstString& dest) {
        manager.connect(src,dest);
    }

    void disconnect(const yarp::os::ConstString& src,
                    const yarp::os::ConstString& dest,
                    bool srcDrop) {
        manager.disconnect(src,dest,srcDrop);
    }

    virtual bool addSubscription(const yarp::os::ConstString& src,
                                 const yarp::os::ConstString& dest,
                                 const yarp::os::ConstString& mode) = 0;

    virtual bool removeSubscription(const yarp::os::ConstString& src,
                                    const yarp::os::ConstString& dest) = 0;

    virtual bool listSubscriptions(const yarp::os::ConstString& src,
                                   yarp::os::Bottle& reply) = 0;

    virtual bool welcome(const yarp::os::ConstString& port, int activity) = 0;

    virtual bool setTopic(const yarp::os::ConstString& port, 
                          const yarp::os::ConstString& structure, bool active) = 0;

    virtual bool listTopics(yarp::os::Bottle& topics) = 0;

    virtual bool setType(const yarp::os::ConstString& family,
                         const yarp::os::ConstString& structure,
                         const yarp::os::ConstString& value) = 0;

    virtual yarp::os::ConstString getType(const yarp::os::ConstString& family,
                                          const yarp::os::ConstString& structure) = 0;

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       yarp::os::Contact& remote) {
        yarp::os::ConstString tag = cmd.get(0).asString();
        bool ok = false;
        if (tag=="subscribe"||tag=="unsubscribe"||tag=="announce"||
            tag=="topic"||tag=="untopic"||tag=="type") {
            printf("-> %s\n", cmd.toString().c_str());
        }
        if (tag=="subscribe") {
            yarp::os::ConstString src = cmd.get(1).asString();
            yarp::os::ConstString dest = cmd.get(2).asString();
            yarp::os::ConstString mode = cmd.get(3).asString();
            if (dest!="") {
                ok = addSubscription(src,
                                     dest,
                                     mode);
                reply.clear();
                reply.addVocab(ok?VOCAB2('o','k'):VOCAB4('f','a','i','l'));
                return ok;
            } else {
                // list subscriptions
                listSubscriptions(src,reply);
            }
            return true;
        }
        if (tag=="unsubscribe") {
            ok = removeSubscription(cmd.get(1).asString().c_str(),
                                    cmd.get(2).asString().c_str());
            reply.clear();
            reply.addVocab(ok?VOCAB2('o','k'):VOCAB4('f','a','i','l'));
            return ok;
        }
        if (tag=="announce") {
            if (cmd.get(2).isInt()) {
                welcome(cmd.get(1).asString().c_str(),cmd.get(2).asInt()?1:0);
            } else {
                welcome(cmd.get(1).asString().c_str(),true);
            }
            reply.clear();
            reply.addVocab(VOCAB2('o','k'));
            return true;
        }
        if (tag=="topic") {
            if (cmd.size()>=2) {
                bool result = setTopic(cmd.get(1).asString().c_str(),
                                       cmd.get(2).asString().c_str(),true);
                reply.clear();
                reply.addVocab(replyCode(result));
                return true;
            } else {
                reply.clear();
                listTopics(reply);
                return true;
            }
        }
        if (tag=="type") {
            if (cmd.size()==4) {
                bool result = setType(cmd.get(1).asString().c_str(),
                                      cmd.get(2).asString().c_str(),
                                      cmd.get(3).asString().c_str());
                reply.clear();
                reply.addVocab(replyCode(result));
                return true;
            } else if (cmd.size()==3) {
                yarp::os::ConstString result = 
                    getType(cmd.get(1).asString().c_str(),
                            cmd.get(2).asString().c_str());
                reply.clear();
                if (result=="") {
                    reply.addVocab(replyCode(false));
                } else {
                    reply.addString(cmd.get(0).asString());
                    reply.addString(cmd.get(1).asString());
                    reply.addString(cmd.get(2).asString());
                    reply.addString(result);
                }
                return true;
            } else {
                reply.clear();
                reply.addVocab(replyCode(false));
                return true;
            }
        }
        if (tag=="untopic") {
            bool result = setTopic(cmd.get(1).asString().c_str(),NULL,false);
            reply.clear();
            reply.addVocab(replyCode(result));
            return true;
        }
        return ok;
    }

    virtual void onEvent(yarp::os::Bottle& event) {
    }

    int replyCode(bool flag) {
        return flag?VOCAB2('o','k'):VOCAB4('f','a','i','l');
    }
};


#endif
