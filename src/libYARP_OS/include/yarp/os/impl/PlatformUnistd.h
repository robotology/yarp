/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMUNISTD_H
#define YARP_OS_IMPL_PLATFORMUNISTD_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/OS_NS_unistd.h>
#else
# include <unistd.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
    using ACE_OS::rmdir;
    inline int gethostname(char *name, size_t len) { return ACE_OS::hostname(name, len); }
    using ACE_OS::getpid;
    using ACE_OS::getppid;
    using ACE_OS::isatty;
    using ACE_OS::getcwd;
    using ACE_OS::fork;
    using ACE_OS::pipe;
    using ACE_OS::dup;
    using ACE_OS::dup2;
    using ACE_OS::execvp;
    using ACE_OS::chdir;
    using ACE_OS::unlink;

    // ACE version of execl/execlp/execle are just fake implementation, see
    // https://github.com/DOCGroup/ACE_TAO/issues/409
    // https://github.com/DOCGroup/ACE_TAO/blob/ACE%2BTAO-6_4_3/ACE/ace/OS_NS_unistd.cpp#L227
    // (last ACE version tested: 6.4.3).
    // This is not a big issue since (at the moment) it is used only in
    // #if !defined(_WIN32) branches, but we might need to fix this at some
    // point.
    using ::execlp;
#else
    using ::rmdir;
    using ::gethostname;
    using ::getpid;
    using ::getppid;
    using ::isatty;
    using ::getcwd;
    using ::fork;
    using ::pipe;
    using ::dup;
    using ::dup2;
    using ::execlp;
    using ::execvp;
    using ::chdir;
    using ::unlink;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMUNISTD_H
