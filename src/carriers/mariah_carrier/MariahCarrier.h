/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Carrier.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>

class MariahCarrier : public yarp::os::Carrier {
public:

    // First, the easy bits...

    Carrier *create() const override;

    std::string getName() const override;

    bool isConnectionless() const override;

    bool canAccept() const override;

    bool canOffer() const override;

    bool isTextMode() const override;

    bool canEscape() const override;

    bool requireAck() const override;

    bool supportReply() const override;

    bool isLocal() const override;

    std::string toString() const override;

    void getHeader(yarp::os::Bytes& header) const override;

    bool sendHeader(yarp::os::ConnectionState& proto) override;

    bool checkHeader(const yarp::os::Bytes& header) override;

    void setParameters(const yarp::os::Bytes& header) override;


    // Now, the initial hand-shaking

    bool prepareSend(yarp::os::ConnectionState& proto) override;

    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override;

    bool expectExtraHeader(yarp::os::ConnectionState& proto) override;

    bool respondToHeader(yarp::os::ConnectionState& proto) override;

    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

    bool isActive() const override;


    // Payload time!

    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    virtual bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer);

    bool expectIndex(yarp::os::ConnectionState& proto) override;

    // Acknowledgements, we don't do them

    bool sendAck(yarp::os::ConnectionState& proto) override;

    bool expectAck(yarp::os::ConnectionState& proto) override;

};
