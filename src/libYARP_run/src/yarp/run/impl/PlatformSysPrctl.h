/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_PLATFORMSYSPRCTL_H
#define YARP_RUN_IMPL_PLATFORMSYSPRCTL_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_SYS_PRCTL_H)
# include <sys/prctl.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_SYS_PRCTL_H)
    using ::prctl;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_RUN_IMPL_PLATFORMSYSPRCTL_H
