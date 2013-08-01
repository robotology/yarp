// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_SUBSCRIBERONSQL_INC
#define YARPDB_SUBSCRIBERONSQL_INC

#include "Subscriber.h"

#include <yarp/os/Semaphore.h>

/**
 *
 * Interface for maintaining persistent connections using SQL.
 *
 */
class SubscriberOnSql : public Subscriber {
public:
    SubscriberOnSql() : mutex(1) {
        implementation = 0/*NULL*/;
        verbose = false;
    }

    virtual ~SubscriberOnSql() {
        if (implementation!=0/*NULL*/) {
            close();
        }
    }

    bool open(const yarp::os::ConstString& filename, bool fresh = false);

    bool close();

    virtual bool addSubscription(const yarp::os::ConstString& src,
                                 const yarp::os::ConstString& dest,
                                 const yarp::os::ConstString& mode);

    virtual bool removeSubscription(const yarp::os::ConstString& src,
                                    const yarp::os::ConstString& dest);

    virtual bool listSubscriptions(const yarp::os::ConstString& port,
                                   yarp::os::Bottle& reply);

    virtual bool welcome(const yarp::os::ConstString& port, int activity);

    bool hookup(const yarp::os::ConstString& port);

    bool breakdown(const yarp::os::ConstString& port);

    virtual bool setTopic(const yarp::os::ConstString& port, 
                          const yarp::os::ConstString& structure, bool active);

    virtual bool listTopics(yarp::os::Bottle& topics);

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
                         const yarp::os::ConstString& value);

    virtual yarp::os::ConstString getType(const yarp::os::ConstString& family,
                                          const yarp::os::ConstString& structure);


    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }

private:
    void *implementation;
    bool verbose;
    yarp::os::Semaphore mutex;
};


#endif
