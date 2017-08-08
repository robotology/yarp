/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
