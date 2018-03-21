/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMDLFCN_H
#define YARP_OS_IMPL_PLATFORMDLFCN_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
# include <ace/OS_NS_dlfcn.h>
#elif defined(YARP_HAS_DLFCN_H)
# include <dlfcn.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
    using ACE_OS::dlerror;
    using ACE_OS::dlopen;
    using ACE_OS::dlclose;
    using ACE_OS::dlsym;
#elif defined(YARP_HAS_DLFCN_H)
    using ::dlerror;
    using ::dlopen;
    using ::dlclose;
    using ::dlsym;
#else
    YARP_COMPILER_WARNING("dlfcn.h not found on this platform")
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMDLFCN_H
