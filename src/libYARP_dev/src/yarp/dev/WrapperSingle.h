/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_WRAPPERSINGLE_H
#define YARP_DEV_WRAPPERSINGLE_H

#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

namespace yarp {
namespace dev {

/**
 *
 */


/**
 * @ingroup dev_iwrapper
 *
 * @brief Helper interface for an object that can wrap/or "attach" to a single
 * other device.
 *
 * The IWrapper methods should be implemented by the user, but the device can
 * be used also through the IMultipleWrapper interface.
 */
class YARP_dev_API WrapperSingle :
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper
{
public:

    /**
     * Destructor.
     */
    ~WrapperSingle() override;

    // yarp::dev::IMultipleWrapper
    bool attachAll(const yarp::dev::PolyDriverList& drivers) final;
    bool detachAll() final;
};


} // namespace dev
} // namespace yarp

#endif // YARP_DEV_WRAPPERSINGLE_H
