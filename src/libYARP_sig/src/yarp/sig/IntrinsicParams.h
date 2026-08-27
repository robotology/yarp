/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_INTRINSICPARAMS_H
#define YARP_SIG_INTRINSICPARAMS_H

#include <yarp/os/Log.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/sig/api.h>
#include <yarp/sig/IntrinsicParamsData.h>

namespace yarp::sig {

/**
 * @brief The IntrinsicParams struct to handle the intrinsic parameter
 * of cameras(RGB and RGBD either).
 */
struct YARP_sig_API IntrinsicParams : public IntrinsicParamsData
{
    /**
     * @brief IntrinsicParams, default constructor
     */
    IntrinsicParams();
};

} // namespace yarp::sig


#endif // YARP_SIG_INTRINSICPARAMS_H
