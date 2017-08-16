/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMSIGNAL_H
#define YARP_OS_IMPL_PLATFORMSIGNAL_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/OS_NS_signal.h>
#elif defined(YARP_HAS_SIGNAL_H)
# include <signal.h>
#elif defined(YARP_HAS_SYS_SIGNAL_H)
# include <sys/signal.h>
#endif
#if defined(YARP_HAS_CSIGNAL)
# include <csignal>
#endif

namespace yarp {
namespace os {
namespace impl {

#if defined(YARP_HAS_ACE)
    using ACE_OS::sigemptyset;
    using ACE_OS::sigfillset;
    using ACE_OS::sigaction;
    using ACE_OS::kill;
    // std::signal is broken for Visual Studio 12 2013
    // (Fixed in Visual studio 14 2015)
#  if defined(_MSC_VER) && _MSC_VER <= 1800
    using ACE_OS::signal;
    using ACE_OS::raise;
# elif defined(YARP_HAS_CSIGNAL)
    // Prefer std::signal over ::signal
    using std::signal;
    using std::raise;
# else
    using ::signal;
    using ::raise;
# endif
#else
#if defined(__APPLE__) && defined(sigfillset)
#undef sigfillset
#endif
    using ::sigemptyset;
    using ::sigfillset;
    using ::sigaction;
    using ::kill;
# if defined(YARP_HAS_CSIGNAL)
    // Prefer std::signal over ::signal
    using std::signal;
    using std::raise;
# else
    using ::signal;
    using ::raise;
# endif
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMSIGNAL_H
