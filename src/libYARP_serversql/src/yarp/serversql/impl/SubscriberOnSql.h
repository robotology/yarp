/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H
#define YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H

#include <yarp/serversql/impl/Subscriber.h>

#include <mutex>


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
    SubscriberOnSql() : mutex() {
        implementation = nullptr;
        verbose = false;
    }

    virtual ~SubscriberOnSql() {
        if (implementation != nullptr) {
            close();
        }
    }

    bool open(const std::string& filename, bool fresh = false);

    bool close();

    virtual bool addSubscription(const std::string& src,
                                 const std::string& dest,
                                 const std::string& mode) override;

    virtual bool removeSubscription(const std::string& src,
                                    const std::string& dest) override;

    virtual bool listSubscriptions(const std::string& port,
                                   yarp::os::Bottle& reply) override;

    bool welcome(const std::string& port, int activity) override;

    bool hookup(const std::string& port);

    bool breakdown(const std::string& port);

    virtual bool setTopic(const std::string& port,
                          const std::string& structure, bool active) override;

    bool listTopics(yarp::os::Bottle& topics) override;

    bool transitiveWelcome(const std::string& topic,
                           const std::string& src,
                           const std::string& dest,
                           const std::string& srcFull,
                           const std::string& destFull);

    bool checkSubscription(const std::string& src,
                           const std::string& dest,
                           const std::string& srcFull,
                           const std::string& destFull,
                           const std::string& mode);

    bool breakSubscription(const std::string& dropper,
                           const std::string& src,
                           const std::string& dest,
                           const std::string& srcFull,
                           const std::string& destFull,
                           const std::string& mode);

    virtual bool setType(const std::string& family,
                         const std::string& structure,
                         const std::string& value) override;

    virtual std::string getType(const std::string& family,
                                          const std::string& structure) override;


    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }

private:
    void *implementation;
    bool verbose;
    std::mutex mutex;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_SUBSCRIBERONSQL_H
