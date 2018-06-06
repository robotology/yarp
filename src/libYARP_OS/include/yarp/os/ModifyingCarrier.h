/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual Carrier *create() const override = 0;

    virtual std::string getName() const override = 0;

    virtual bool checkHeader(const yarp::os::Bytes &header) override;
    virtual void getHeader(yarp::os::Bytes &header) const override;
    virtual bool respondToHeader(yarp::os::ConnectionState& proto) override;
    virtual bool modifiesIncomingData() const override;
    virtual bool modifiesOutgoingData() const override;
    virtual bool modifiesReply() const override;
    virtual void setCarrierParams(const yarp::os::Property& params) override;
    virtual void getCarrierParams(yarp::os::Property& params) const override;
    virtual bool configureFromProperty(yarp::os::Property& prop) override;
    // only remains to implement modifyIncomingData()
};

#endif // YARP_OS_MODIFYINGCARRIER_H
