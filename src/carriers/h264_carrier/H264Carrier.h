/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
 * This carrier uses gstreamer libraries to read rtp packets and to decode the h264 stream.
 *
 * Use this carrier in the following way:
 * - suppose there is a server that streams video frames to IP x.x.x.x and to port p:
 *   register this port to yarp by yarp command "yarp name register" in this way: yarp name register /serverH264Stream h264 x.x.x.x p
 * - you need to connect your client port (for example /yarpview/img:i ) to /serverH264Stream port by h264 carrier to get the video stream:
 *   yarp connect /serverH264Stream /yarpview/img:i h264
 * You can comfigure the carrier to cro frames by passing parameters to the carrier with usual syntax: +pramName.paramValue:
 *  - +cropLeft.100    ==> the carrier crops 100 pxel from left side
 *  - +cropRight.100   ==> the carrier crops 100 pxel from right side
 *  - +cropTop.100     ==> the carrier crops 100 pxel from top side
 *  - +cropBottom.100  ==> the carrier crops 100 pxel from bottom side
 *  - +verbose.1       ==> enables verbose mode (default is not verbose) (+verbose.0 disables it.)
 */

class yarp::os::H264Carrier : public Carrier
{
private:
    bool decoderIsRunning;
    std::string envelope;
    h264Decoder_cfgParamters cfgParams;
public:
    H264Carrier()
    {;}

    virtual Carrier *create() override
    {
        return new H264Carrier();
    }

    virtual std::string getName() const override;

    virtual bool isConnectionless() const override;

    virtual bool canAccept() override;

    virtual bool canOffer() override;

    virtual bool isTextMode() const override;

    virtual bool canEscape() const override;

    virtual void handleEnvelope(const std::string& envelope) override;

    virtual bool requireAck() override;

    virtual bool supportReply() override;

    virtual bool isLocal() override;

    // this is important - flips expected flow of messages
    virtual bool isPush() const override;

    virtual std::string toString() override;

    virtual void getHeader(const Bytes& header) override;

    virtual bool checkHeader(const Bytes& header) override;

    virtual void setParameters(const Bytes& header) override;


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override;

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override;

    virtual bool expectExtraHeader(ConnectionState& proto) override;

    bool respondToHeader(ConnectionState& proto) override;

    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    virtual bool isActive() const override;


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    virtual bool expectIndex(ConnectionState& proto) override;

    virtual bool sendAck(ConnectionState& proto) override;

    virtual bool expectAck(ConnectionState& proto) override;

    virtual std::string getBootstrapCarrierName() override;

    virtual yarp::os::Face* createFace(void) override;

};

#endif
