/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_WRAPPERMULTIPLE_H
#define YARP_DEV_WRAPPERMULTIPLE_H

#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iwrapper
 *
 * @brief Helper interface for an object that can wrap/or "attach" to one or
 * more other devices.
 *
 * The IMultipleWrapper methods should be implemented by the user, but the
 * device can be used also through the IWrapper interface when only a single
 * device should be passed.
 */
class YARP_dev_API WrapperMultiple :
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper
{
public:

    /**
     * Destructor.
     */
    ~WrapperMultiple() override;

    // yarp::dev::IWrapper
    bool attach(PolyDriver *driver) final;
    bool detach() final;
};


} // namespace dev
} // namespace yarp

#endif // YARP_DEV_WRAPPERMULTIPLE_H
