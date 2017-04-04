/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

#ifdef YARP_HAS_ACE
    using ACE_OS::rmdir;
    inline int gethostname(char *name, size_t len) { return ACE_OS::hostname(name, len); }
    using ACE_OS::getpid;
    using ACE_OS::isatty;
    using ACE_OS::getcwd;
    using ACE_OS::fork;
#else
    using ::rmdir;
    using ::gethostname;
    using ::getpid;
    using ::isatty;
    using ::getcwd;
    using ::fork;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMUNISTD_H
