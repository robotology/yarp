/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MONITORBINDING_INC
#define MONITORBINDING_INC

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Property.h>
#include <yarp/os/Things.h>

#include "MonitorEvent.h"

class MonitorBinding
{

public:
    virtual ~MonitorBinding();
    virtual bool load(const yarp::os::Property& options) = 0;
    virtual bool setParams(const yarp::os::Property& params) = 0;
    virtual bool getParams(yarp::os::Property& params) = 0;

    virtual bool hasAccept() = 0;
    virtual bool acceptData(yarp::os::Things& thing) = 0;

    virtual bool hasUpdate() = 0;
    virtual yarp::os::Things& updateData(yarp::os::Things& thing) = 0;

    virtual bool hasUpdateReply() = 0;
    virtual yarp::os::Things& updateReply(yarp::os::Things& thing) = 0;

    virtual bool peerTrigged() = 0;
    virtual bool setAcceptConstraint(const char* constraint) = 0;
    virtual const char* getAcceptConstraint() = 0;
    virtual bool canAccept() = 0;

    /**
     * factory method
     */
    static MonitorBinding *create(const char* script_type);

};

#endif //MONITORBINDING_INC_
