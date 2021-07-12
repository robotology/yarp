/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PLATFORMSIGNAL_H
#define YARP_OS_IMPL_PLATFORMSIGNAL_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/OS_NS_signal.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(YARP_HAS_SIGNAL_H)
#    include <signal.h>
#elif defined(YARP_HAS_SYS_SIGNAL_H)
#    include <sys/signal.h>
#endif
#if defined(YARP_HAS_CSIGNAL)
#    include <csignal>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
using ACE_OS::kill;
using ACE_OS::sigaction;
using ACE_OS::sigemptyset;
using ACE_OS::sigfillset;
#    if defined(YARP_HAS_CSIGNAL)
// Prefer std::signal over ::signal
using std::raise;
using std::signal;
#    else
using ::raise;
using ::signal;
#    endif
#else
#    if defined(__APPLE__) && defined(sigfillset)
#        undef sigfillset
#    endif
using ::kill;
using ::sigaction;
using ::sigemptyset;
using ::sigfillset;
#    if defined(YARP_HAS_CSIGNAL)
// Prefer std::signal over ::signal
using std::raise;
using std::signal;
#    else
using ::raise;
using ::signal;
#    endif
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMSIGNAL_H
