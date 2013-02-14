// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MODIFYINGCARRIER_
#define _YARP2_MODIFYINGCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/impl/Protocol.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ModifyingCarrier;
        }
    }
}

/**
 * A starter class for implementing simple modifying carriers.
 * These are not standalone carriers, they just tweak payload data
 * in custom ways.
 */
class YARP_OS_impl_API yarp::os::impl::ModifyingCarrier : public AbstractCarrier {
public:

    virtual Carrier *create() = 0;

    virtual String getName() = 0;

    virtual bool checkHeader(const yarp::os::Bytes &header);
    virtual void getHeader(const yarp::os::Bytes &header);
    virtual bool respondToHeader(Protocol& proto);
    virtual bool modifiesIncomingData();
    virtual void setCarrierParams(const yarp::os::Property& params);
    virtual void getCarrierParams(yarp::os::Property& params);

    // only remains to implement modifyIncomingData()
};

#endif
