/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_MODIFYINGCARRIER_H
#define YARP_OS_MODIFYINGCARRIER_H

#include <yarp/os/AbstractCarrier.h>

namespace yarp {
namespace os {

/**
 * A starter class for implementing simple modifying carriers.
 * These are not standalone carriers, they just tweak payload data
 * in custom ways.
 */
class YARP_os_API ModifyingCarrier : public AbstractCarrier
{
public:
    Carrier* create() const override = 0;

    std::string getName() const override = 0;

    bool checkHeader(const yarp::os::Bytes& header) override;
    void getHeader(yarp::os::Bytes& header) const override;
    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool modifiesIncomingData() const override;
    bool modifiesOutgoingData() const override;
    bool modifiesReply() const override;
    void setCarrierParams(const yarp::os::Property& params) override;
    void getCarrierParams(yarp::os::Property& params) const override;
    bool configureFromProperty(yarp::os::Property& prop) override;
    // only remains to implement modifyIncomingData()
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_MODIFYINGCARRIER_H
