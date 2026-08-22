/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MOBILEBASEVELOCITY_H
#define YARP_DEV_MOBILEBASEVELOCITY_H

#include <yarp/dev/MobileBaseVelocityData.h>

namespace yarp::dev::Nav2D {
class YARP_dev_API MobileBaseVelocity : public yarp::dev::MobileBaseVelocityData
{
public:

    MobileBaseVelocity()=default;
    virtual ~MobileBaseVelocity()=default;
};

} // namespace yarp::dev::Nav2D

#endif // YARP_DEV_MOBILEBASEVELOCITY_H
