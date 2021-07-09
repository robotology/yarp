/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef H264CARRIER_INC
#define H264CARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/Face.h>
#include "H264Decoder.h"


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

class H264Carrier :
        public yarp::os::Carrier
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

    void getHeader(yarp::os::Bytes& header) const override;

    bool checkHeader(const yarp::os::Bytes& header) override;

    void setParameters(const yarp::os::Bytes& header) override;


    // Now, the initial hand-shaking

    bool prepareSend(yarp::os::ConnectionState& proto) override;

    bool sendHeader(yarp::os::ConnectionState& proto) override;

    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override;

    bool expectExtraHeader(yarp::os::ConnectionState& proto) override;

    bool respondToHeader(yarp::os::ConnectionState& proto) override;

    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

    bool isActive() const override;


    // Payload time!

    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    bool reply(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    virtual bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer);

    bool expectIndex(yarp::os::ConnectionState& proto) override;

    bool sendAck(yarp::os::ConnectionState& proto) override;

    bool expectAck(yarp::os::ConnectionState& proto) override;

    std::string getBootstrapCarrierName() const override;

    yarp::os::Face* createFace() const override;

};

#endif
