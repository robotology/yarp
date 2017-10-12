/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Valentina Gaggero <valentina.gaggero@iit.it>
 * Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef H264CARRIER_INC
#define H264CARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/Face.h>


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
 *   register this port to yarp by yarp command "yarp register" in this way: yarp register /serverH264Stream h264 x.x.x.x p
 * - you need to connect your client port (for example /yarpview/img:i ) to /serverH264Stream port by h264 carrier to get the video stream:
 *   yarp connect /serverH264Stream /yarpview/img:i h264
 */

class yarp::os::H264Carrier : public Carrier
{
private:
    bool decoderIsRunning;
    yarp::os::ConstString envelope;
public:
    H264Carrier()
    {;}

    virtual Carrier *create() override
    {
        return new H264Carrier();
    }

    virtual ConstString getName() override;

    virtual bool isConnectionless() override;

    virtual bool canAccept() override;

    virtual bool canOffer() override;

    virtual bool isTextMode() override;

    virtual bool canEscape() override;

    virtual void handleEnvelope(const yarp::os::ConstString& envelope) override;

    virtual bool requireAck() override;

    virtual bool supportReply() override;

    virtual bool isLocal() override;

    // this is important - flips expected flow of messages
    virtual bool isPush() override;

    virtual ConstString toString() override;

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

    virtual bool isActive() override;


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    virtual bool expectIndex(ConnectionState& proto) override;

    virtual bool sendAck(ConnectionState& proto) override;

    virtual bool expectAck(ConnectionState& proto) override;

    virtual ConstString getBootstrapCarrierName() override;

    virtual yarp::os::Face* createFace(void) override;
};

#endif
