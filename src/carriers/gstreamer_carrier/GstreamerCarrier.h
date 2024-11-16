/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_GSTREAMERCARRIER_H
#define YARP_GSTREAMERCARRIER_H

#include <yarp/os/AbstractCarrier.h>

/**
 * GstreamerCarrier
 */
class GstreamerCarrier :
        public yarp::os::Carrier
{
    std::string pipeline_string;

public:
    GstreamerCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    bool checkHeader(const yarp::os::Bytes& header) override;
    void getHeader(yarp::os::Bytes& header) const override;
    void setParameters(const yarp::os::Bytes& header) override;
    bool requireAck() const override  {        return false;    }
    bool isConnectionless() const override;
    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

    ///
    bool isPush() const override    {    return false; }
    bool canAccept() const override  {    return true; }
    bool canOffer() const override   {    return true; }
    bool isTextMode() const override {    return false; }
    bool canEscape() const override {    return false; }

    bool supportReply() const override {    return false; }

    bool isLocal() const override {    return false; }

    bool prepareSend(yarp::os::ConnectionState& proto) override;
    bool sendHeader(yarp::os::ConnectionState& proto) override;
    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override;
    bool expectExtraHeader(yarp::os::ConnectionState& proto) override;
    bool isActive() const override
    {
        return true;
    }

    bool expectAck(yarp::os::ConnectionState& proto) override;
    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;
    bool reply(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;
    std::string toString() const override;
    bool expectIndex(yarp::os::ConnectionState& proto) override;

    bool sendAck(yarp::os::ConnectionState& proto) override;

    yarp::os::Face* createFace() const override;
};


/*example:
gst-launch-1.0 -v videotestsrc ! "video/x-raw, format=(string)RGB, width=(int)640, height=(int)480" ! videoconvert ! openh264enc ! h264parse ! rtph264pay pt=96 config-interval=5 ! udpsink host=127.0.0.1 port=15000
gst-launch-1.0 -v videotestsrc ! "video/x-raw, format=(string)I420, width=(int)640, height=(int)480" ! openh264enc ! h264parse ! udpsink host = 127.0.0.1 port = 15000
yarp name register /gst gstreamer 127.0.0.1 15000
yarp read /ccc //DEBUG
yarp connect /gst /ccc gstreamer

*/

#endif
