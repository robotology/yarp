/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PLATFORMDIRENT_H
#define YARP_OS_IMPL_PLATFORMDIRENT_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
#    include <ace/OS_NS_dirent.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <dirent.h>
#    include <sys/types.h>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
typedef ACE_DIRENT dirent;
typedef ACE_DIR DIR;
using ACE_OS::closedir;
using ACE_OS::opendir;
using ACE_OS::scandir;
inline int alphasort(const ACE_DIRENT** f1, const ACE_DIRENT** f2)
{
    return ACE_OS::alphasort(f1, f2);
}
#else
using ::alphasort;
using ::closedir;
using ::DIR;
using ::dirent;
using ::opendir;
using ::scandir;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMDIRENT_H
