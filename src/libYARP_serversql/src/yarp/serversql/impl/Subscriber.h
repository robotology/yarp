/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_SUBSCRIBER_H
#define YARP_SERVERSQL_IMPL_SUBSCRIBER_H

#include <yarp/name/NameService.h>
#include <yarp/serversql/impl/ConnectThread.h>

#include <string>
#include <yarp/os/Vocab.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/NameSpace.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 *
 * Abstract interface for maintaining persistent connections.
 *
 */
class Subscriber : public yarp::name::NameService {
private:
    yarp::os::NameStore *store;
    ConnectManager manager;
    bool silent;
    yarp::os::NameSpace *delegate;
public:
    Subscriber() :
            store(nullptr),
            silent(false),
            delegate(nullptr) {
    }

    void setStore(yarp::os::NameStore& store) { this->store = &store; }
    yarp::os::NameStore *getStore() { return store; }

    void setSilent(bool flag) {
        this->silent = flag;
    }

    void clear() {
        manager.clear();
    }

    void connect(const std::string& src,
                 const std::string& dest) {
        manager.connect(src,dest);
    }

    void disconnect(const std::string& src,
                    const std::string& dest,
                    bool srcDrop) {
        manager.disconnect(src,dest,srcDrop);
    }

    virtual bool addSubscription(const std::string& src,
                                 const std::string& dest,
                                 const std::string& mode) = 0;

    virtual bool removeSubscription(const std::string& src,
                                    const std::string& dest) = 0;

    virtual bool listSubscriptions(const std::string& src,
                                   yarp::os::Bottle& reply) = 0;

    virtual bool welcome(const std::string& port, int activity) = 0;

    virtual bool setTopic(const std::string& port,
                          const std::string& structure, bool active) = 0;

    virtual bool listTopics(yarp::os::Bottle& topics) = 0;

    virtual bool setType(const std::string& family,
                         const std::string& structure,
                         const std::string& value) = 0;

    virtual std::string getType(const std::string& family,
                                          const std::string& structure) = 0;

    virtual bool apply(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) override {
        std::string tag = cmd.get(0).asString();
        bool ok = false;
        if (tag=="subscribe"||tag=="unsubscribe"||tag=="announce"||
            tag=="topic"||tag=="untopic"||tag=="type") {
            if (!silent) printf("-> %s\n", cmd.toString().c_str());
        }
        if (tag=="subscribe") {
            std::string src = cmd.get(1).asString();
            std::string dest = cmd.get(2).asString();
            std::string mode = cmd.get(3).asString();
            if (dest!="") {
                ok = addSubscription(src,
                                     dest,
                                     mode);
                reply.clear();
                reply.addVocab(ok?yarp::os::createVocab('o','k'):yarp::os::createVocab('f','a','i','l'));
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
            reply.addVocab(ok?yarp::os::createVocab('o','k'):yarp::os::createVocab('f','a','i','l'));
            return ok;
        }
        if (tag=="announce") {
            if (cmd.get(2).isInt32()) {
                welcome(cmd.get(1).asString().c_str(),cmd.get(2).asInt32()?1:0);
            } else {
                welcome(cmd.get(1).asString().c_str(),true);
            }
            reply.clear();
            reply.addVocab(yarp::os::createVocab('o','k'));
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
                std::string result =
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
            bool result = setTopic(cmd.get(1).asString().c_str(),"",false);
            reply.clear();
            reply.addVocab(replyCode(result));
            return true;
        }
        return ok;
    }

    void onEvent(yarp::os::Bottle& event) override {
    }

    int replyCode(bool flag) {
        return flag?yarp::os::createVocab('o','k'):yarp::os::createVocab('f','a','i','l');
    }

    void setDelegate(yarp::os::NameSpace *delegate) {
        this->delegate = delegate;
    }

    yarp::os::NameSpace *getDelegate() {
        return delegate;
    }
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_SUBSCRIBER_H
