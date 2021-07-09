/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ABSTRACTCARRIER_H
#define YARP_OS_ABSTRACTCARRIER_H

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>

namespace yarp {
namespace os {

/**
 * \brief A starter class for implementing simple carriers.
 *
 * It implements reasonable default behavior.
 */
class YARP_os_API AbstractCarrier : public Carrier
{
public:
    /** @{ */

    // Documented in Carrier
    Carrier* create() const override = 0;

    // Documented in Carrier
    std::string getName() const override = 0;

    // Documented in Carrier
    bool checkHeader(const yarp::os::Bytes& header) override = 0;

    // Documented in Carrier
    void setParameters(const yarp::os::Bytes& header) override;

    // Documented in Carrier
    void getHeader(yarp::os::Bytes& header) const override = 0;

    // Documented in Carrier
    void setCarrierParams(const yarp::os::Property& params) override;
    // Documented in Carrier
    void getCarrierParams(yarp::os::Property& params) const override;

    /** @} */
    /** @{ */

    // Documented in Carrier
    bool isConnectionless() const override;
    // Documented in Carrier
    bool supportReply() const override;
    // Documented in Carrier
    bool canAccept() const override;
    // Documented in Carrier
    bool canOffer() const override;
    // Documented in Carrier
    bool isTextMode() const override;
    // Documented in Carrier
    bool requireAck() const override;
    // Documented in Carrier
    bool canEscape() const override;
    // Documented in Carrier
    bool isLocal() const override;
    // Documented in Carrier
    std::string toString() const override;

    // Documented in Carrier
    bool isActive() const override;

    /** @} */
    /** @{ */
    // Sender methods

    // Documented in Carrier
    bool prepareSend(ConnectionState& proto) override;
    // Documented in Carrier
    bool sendHeader(ConnectionState& proto) override;
    // Documented in Carrier
    bool expectReplyToHeader(ConnectionState& proto) override;
    // Documented in Carrier
    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    /** @} */
    /** @{ */
    // Receiver methods

    // Documented in Carrier
    bool expectExtraHeader(ConnectionState& proto) override;
    // Documented in Carrier
    bool respondToHeader(ConnectionState& proto) override = 0; // left abstract, no good default
    // Documented in Carrier
    bool expectIndex(ConnectionState& proto) override;
    // Documented in Carrier
    bool expectSenderSpecifier(ConnectionState& proto) override;
    // Documented in Carrier
    bool sendAck(ConnectionState& proto) override;
    // Documented in Carrier
    bool expectAck(ConnectionState& proto) override;

    /** @} */
    /** @{ */
    // some default implementations of protocol phases used by
    // certain YARP carriers

    /**
     * Default implementation for the sendHeader method
     */
    bool defaultSendHeader(ConnectionState& proto);

    /**
     * Default implementation for the expectIndex method
     */
    bool defaultExpectIndex(ConnectionState& proto);

    /**
     * Default implementation for the sendIndex method
     */
    bool defaultSendIndex(ConnectionState& proto, SizedWriter& writer);

    /**
     * Default implementation for the expectAck method
     */
    bool defaultExpectAck(ConnectionState& proto);

    /**
     * Default implementation for the sendAck method
     */
    bool defaultSendAck(ConnectionState& proto);

    /**
     * Read 8 bytes and interpret them as a YARP number
     */
    int readYarpInt(ConnectionState& proto);

    /**
     * Write \c n as an 8 bytes yarp number
     */
    void writeYarpInt(int n, ConnectionState& proto);

    /** @} */

protected:
    /** @{ */

    int getSpecifier(const Bytes& b) const;
    void createStandardHeader(int specifier, yarp::os::Bytes& header) const;

    // Documented in Carrier
    bool write(ConnectionState& proto, SizedWriter& writer) override;

    bool sendConnectionStateSpecifier(ConnectionState& proto);
    bool sendSenderSpecifier(ConnectionState& proto);

    /** @} */
    /** @{ */

    static int interpretYarpNumber(const yarp::os::Bytes& b);
    static void createYarpNumber(int x, yarp::os::Bytes& header);

    /** @} */
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ABSTRACTCARRIER_H
