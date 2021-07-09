/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_PLATFORMUNISTD_H
#define YARP_RUN_IMPL_PLATFORMUNISTD_H

#include <yarp/os/impl/PlatformUnistd.h>

namespace yarp {
namespace run {
namespace impl {

#if defined(YARP_HAS_ACE)
    using ACE_OS::fork;
    using ACE_OS::pipe;
    using ACE_OS::dup;
    using ACE_OS::dup2;
    using ACE_OS::execvp;

    // ACE version of execl/execlp/execle are just fake implementation, see
    // https://github.com/DOCGroup/ACE_TAO/issues/409
    // https://github.com/DOCGroup/ACE_TAO/blob/ACE%2BTAO-6_4_3/ACE/ace/OS_NS_unistd.cpp#L227
    // (last ACE version tested: 6.4.3).
    // This is not a big issue since (at the moment) it is used only in
    // #if !defined(_WIN32) branches, but we might need to fix this at some
    // point.
    using ::execlp;
#else
    using ::fork;
    using ::pipe;
    using ::dup;
    using ::dup2;
    using ::execlp;
    using ::execvp;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_RUN_IMPL_PLATFORMUNISTD_H
