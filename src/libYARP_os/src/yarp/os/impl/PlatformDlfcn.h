/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PLATFORMDLFCN_H
#define YARP_OS_IMPL_PLATFORMDLFCN_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
#    include <ace/OS_NS_dlfcn.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(YARP_HAS_DLFCN_H)
#    include <dlfcn.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
using ACE_OS::dlclose;
using ACE_OS::dlerror;
using ACE_OS::dlopen;
using ACE_OS::dlsym;
#elif defined(YARP_HAS_DLFCN_H)
using ::dlclose;
using ::dlerror;
using ::dlopen;
using ::dlsym;
#else
YARP_COMPILER_WARNING("dlfcn.h not found on this platform")
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMDLFCN_H
