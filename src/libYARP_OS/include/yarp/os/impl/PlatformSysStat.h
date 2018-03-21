/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_OS_IMPL_PLATFORMSYSSTAT_H
#define YARP_OS_IMPL_PLATFORMSYSSTAT_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/OS_NS_sys_stat.h>
#else
# include <sys/stat.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
    typedef ACE_stat YARP_stat;
    using ACE_OS::stat;
    using ACE_OS::mkdir;
#else
    typedef struct ::stat YARP_stat;
    using ::stat;
    using ::mkdir;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMSYSSTAT_H
