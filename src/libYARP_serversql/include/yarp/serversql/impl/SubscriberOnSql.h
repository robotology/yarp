/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H
#define YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H

#include <yarp/serversql/impl/Subscriber.h>

#include <yarp/os/Semaphore.h>


namespace yarp {
namespace serversql {
namespace impl {

/**
 *
 * Interface for maintaining persistent connections using SQL.
 *
 */
class SubscriberOnSql : public Subscriber {
public:
    SubscriberOnSql() : mutex(1) {
        implementation = nullptr;
        verbose = false;
    }

    virtual ~SubscriberOnSql() {
        if (implementation != nullptr) {
            close();
        }
    }

    bool open(const yarp::os::ConstString& filename, bool fresh = false);

    bool close();

    virtual bool addSubscription(const yarp::os::ConstString& src,
                                 const yarp::os::ConstString& dest,
                                 const yarp::os::ConstString& mode) override;

    virtual bool removeSubscription(const yarp::os::ConstString& src,
                                    const yarp::os::ConstString& dest) override;

    virtual bool listSubscriptions(const yarp::os::ConstString& port,
                                   yarp::os::Bottle& reply) override;

    virtual bool welcome(const yarp::os::ConstString& port, int activity) override;

    bool hookup(const yarp::os::ConstString& port);

    bool breakdown(const yarp::os::ConstString& port);

    virtual bool setTopic(const yarp::os::ConstString& port,
                          const yarp::os::ConstString& structure, bool active) override;

    virtual bool listTopics(yarp::os::Bottle& topics) override;

    bool transitiveWelcome(const yarp::os::ConstString& topic,
                           const yarp::os::ConstString& src,
                           const yarp::os::ConstString& dest,
                           const yarp::os::ConstString& srcFull,
                           const yarp::os::ConstString& destFull);

    bool checkSubscription(const yarp::os::ConstString& src,
                           const yarp::os::ConstString& dest,
                           const yarp::os::ConstString& srcFull,
                           const yarp::os::ConstString& destFull,
                           const yarp::os::ConstString& mode);

    bool breakSubscription(const yarp::os::ConstString& dropper,
                           const yarp::os::ConstString& src,
                           const yarp::os::ConstString& dest,
                           const yarp::os::ConstString& srcFull,
                           const yarp::os::ConstString& destFull,
                           const yarp::os::ConstString& mode);

    virtual bool setType(const yarp::os::ConstString& family,
                         const yarp::os::ConstString& structure,
                         const yarp::os::ConstString& value) override;

    virtual yarp::os::ConstString getType(const yarp::os::ConstString& family,
                                          const yarp::os::ConstString& structure) override;


    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }

private:
    void *implementation;
    bool verbose;
    yarp::os::Semaphore mutex;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H
