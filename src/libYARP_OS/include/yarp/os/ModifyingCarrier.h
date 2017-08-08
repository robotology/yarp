/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_MODIFYINGCARRIER_H
#define YARP_OS_MODIFYINGCARRIER_H

#include <yarp/os/AbstractCarrier.h>

namespace yarp {
    namespace os {
        class ModifyingCarrier;
    }
}

/**
 * A starter class for implementing simple modifying carriers.
 * These are not standalone carriers, they just tweak payload data
 * in custom ways.
 */
class YARP_OS_API yarp::os::ModifyingCarrier : public AbstractCarrier {
public:

    virtual Carrier *create() YARP_OVERRIDE = 0;

    virtual ConstString getName() YARP_OVERRIDE = 0;

    virtual bool checkHeader(const yarp::os::Bytes &header) YARP_OVERRIDE;
    virtual void getHeader(const yarp::os::Bytes &header) YARP_OVERRIDE;
    virtual bool respondToHeader(yarp::os::ConnectionState& proto) YARP_OVERRIDE;
    virtual bool modifiesIncomingData() YARP_OVERRIDE;
    virtual bool modifiesOutgoingData() YARP_OVERRIDE;
    virtual bool modifiesReply() YARP_OVERRIDE;
    virtual void setCarrierParams(const yarp::os::Property& params) YARP_OVERRIDE;
    virtual void getCarrierParams(yarp::os::Property& params) YARP_OVERRIDE;
    virtual bool configureFromProperty(yarp::os::Property& prop) YARP_OVERRIDE;
    // only remains to implement modifyIncomingData()
};

#endif // YARP_OS_MODIFYINGCARRIER_H
