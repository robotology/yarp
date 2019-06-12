/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef H264CARRIER_INC
#define H264CARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/Face.h>
#include "H264Decoder.h"


namespace yarp {
    namespace os {
        class H264Carrier;
    }
}

/**
 *
 * A carrier for receiving frames compressed in h264 over rtp.
 * This carrier uses gstreamer libraries (libgstreamer1.0-dev and libgstreamer-plugins-base1.0-dev) to read rtp packets and to decode the h264 stream.
 *
 * Use this carrier in the following way:
 * - suppose there is a server that streams video frames to IP x.x.x.x and to port p:
 *   register this port to yarp by yarp command "yarp name register" in this way: yarp name register /serverH264Stream h264 x.x.x.x p
 * - you need to connect your client port (for example /yarpview/img:i ) to /serverH264Stream port by h264 carrier to get the video stream:
 *   yarp connect /serverH264Stream /yarpview/img:i h264
 * You can configure the carrier to crop frames by passing parameters to the carrier with usual syntax: +pramName.paramValue:
 *  - +cropLeft.100    ==> the carrier crops 100 pxel from left side
 *  - +cropRight.100   ==> the carrier crops 100 pxel from right side
 *  - +cropTop.100     ==> the carrier crops 100 pxel from top side
 *  - +cropBottom.100  ==> the carrier crops 100 pxel from bottom side
 *  - +removeJitter.1  ==> the carrier removes the jitter. If you put 0, the jitter is not removed (default behaviour).
 *  - +verbose.1       ==> enables verbose mode (default is not verbose) (+verbose.0 disables it.)
 */

class yarp::os::H264Carrier : public Carrier
{
private:
    std::string envelope;
    h264Decoder_cfgParamters cfgParams;
public:
    H264Carrier()
    {}

    Carrier *create() const override
    {
        return new H264Carrier();
    }

    std::string getName() const override;

    bool isConnectionless() const override;

    bool canAccept() const override;

    bool canOffer() const override;

    bool isTextMode() const override;

    bool canEscape() const override;

    void handleEnvelope(const std::string& envelope) override;

    bool requireAck() const override;

    bool supportReply() const override;

    bool isLocal() const override;

    // this is important - flips expected flow of messages
    bool isPush() const override;

    std::string toString() const override;

    void getHeader(Bytes& header) const override;

    bool checkHeader(const Bytes& header) override;

    void setParameters(const Bytes& header) override;


    // Now, the initial hand-shaking

    bool prepareSend(ConnectionState& proto) override;

    bool sendHeader(ConnectionState& proto) override;

    bool expectSenderSpecifier(ConnectionState& proto) override;

    bool expectExtraHeader(ConnectionState& proto) override;

    bool respondToHeader(ConnectionState& proto) override;

    bool expectReplyToHeader(ConnectionState& proto) override;

    bool isActive() const override;


    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) override;

    bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    bool expectIndex(ConnectionState& proto) override;

    bool sendAck(ConnectionState& proto) override;

    bool expectAck(ConnectionState& proto) override;

    std::string getBootstrapCarrierName() const override;

    yarp::os::Face* createFace(void) const override;

};

#endif
