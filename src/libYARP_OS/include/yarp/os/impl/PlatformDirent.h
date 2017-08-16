/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMDIRENT_H
#define YARP_OS_IMPL_PLATFORMDIRENT_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
# include <ace/OS_NS_dirent.h>
#else
# include <sys/types.h>
# include <dirent.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
    typedef ACE_DIRENT dirent;
    typedef ACE_DIR DIR;
    using ACE_OS::opendir;
    using ACE_OS::closedir;
    using ACE_OS::scandir;
    inline int alphasort(const ACE_DIRENT **f1, const ACE_DIRENT **f2) { return ACE_OS::alphasort(f1, f2); }
#else
    using ::dirent;
    using ::DIR;
    using ::opendir;
    using ::closedir;
    using ::scandir;
    using ::alphasort;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMDIRENT_H
