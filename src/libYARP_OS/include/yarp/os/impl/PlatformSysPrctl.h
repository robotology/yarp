/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMSYSPRCTL_H
#define YARP_OS_IMPL_PLATFORMSYSPRCTL_H

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

#endif // YARP_OS_IMPL_PLATFORMSYSPRCTL_H
