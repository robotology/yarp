// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_SUBSCRIBERONSQL_INC
#define YARPDB_SUBSCRIBERONSQL_INC

#include "Subscriber.h"

/**
 *
 * Interface for maintaining persistent connections using SQL.
 *
 */
class SubscriberOnSql : public Subscriber {
public:
    SubscriberOnSql() {
        implementation = 0/*NULL*/;
        verbose = false;
    }

    virtual ~SubscriberOnSql() {
        if (implementation!=0/*NULL*/) {
            close();
        }
    }

    bool open(const char *filename, bool fresh = false);

    bool close();

    virtual bool addSubscription(const char *src,
                                 const char *dest);

    virtual bool removeSubscription(const char *src,
                                    const char *dest);

    virtual bool listSubscriptions(const char *port,
                                   yarp::os::Bottle& reply);

    virtual bool welcome(const char *port);

    virtual bool setTopic(const char *port, bool active);

    virtual bool listTopics(yarp::os::Bottle& topics);

    bool transitiveWelcome(const char *topic,
                           const char *src, const char *dest,
                           const char *srcFull, const char *destFull);

    bool checkSubscription(const char *src, const char *dest,
                           const char *srcFull, const char *destFull);

    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }

private:
    void *implementation;
    bool verbose;
};


#endif
