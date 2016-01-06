// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_TCPCARRIER
#define YARP2_TCPCARRIER

#include <yarp/os/AbstractCarrier.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TcpCarrier;
        }
    }
}

/**
 * Communicating between two ports via TCP.
 */
class yarp::os::impl::TcpCarrier : public AbstractCarrier {
public:

    TcpCarrier(bool requireAckFlag = true);

    virtual Carrier *create();

    virtual ConstString getName();

    virtual int getSpecifierCode();

    virtual bool checkHeader(const yarp::os::Bytes& header);
    virtual void getHeader(const yarp::os::Bytes& header);
    virtual void setParameters(const yarp::os::Bytes& header);
    virtual bool requireAck();
    virtual bool isConnectionless();
    virtual bool respondToHeader(yarp::os::ConnectionState& proto);
    virtual bool expectReplyToHeader(yarp::os::ConnectionState& proto);

private:
    bool requireAckFlag;
};

#endif

