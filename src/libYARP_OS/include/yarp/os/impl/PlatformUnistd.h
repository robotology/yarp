/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMUNISTD_H
#define YARP_OS_IMPL_PLATFORMUNISTD_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/OS_NS_unistd.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <unistd.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
using ACE_OS::rmdir;
inline int gethostname(char* name, size_t len)
{
    return ACE_OS::hostname(name, len);
}
using ACE_OS::chdir;
using ACE_OS::getcwd;
using ACE_OS::getpid;
using ACE_OS::getppid;
using ACE_OS::isatty;
using ACE_OS::unlink;
#else
using ::chdir;
using ::getcwd;
using ::gethostname;
using ::getpid;
using ::getppid;
using ::isatty;
using ::rmdir;
using ::unlink;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMUNISTD_H
