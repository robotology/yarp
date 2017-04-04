/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PLATFORMSIGNAL_H
#define YARP_OS_IMPL_PLATFORMSIGNAL_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
# include <ace/OS_NS_signal.h>
#elif defined(YARP_HAS_SIGNAL_H)
# include <signal.h>
#elif definef(YARP_HAS_SYS_SIGNAL_H)
# include <sys/signal.h>
#endif

#include <csignal>

namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
    using ACE_OS::sigemptyset;
    using ACE_OS::sigaction;
    using ACE_OS::kill;
#else
    using ::sigemptyset;
    using ::sigaction;
    using ::kill;
#endif

// std::signal is broken for Visual Studio 12 2013
// (Fixed in Visual studio 14 2015)
#if defined(_MSC_VER) && _MSC_VER <= 1800
# ifdef YARP_HAS_ACE
    using ACE_OS::signal;
# else
    YARP_COMPILER_ERROR("signal not defined on this platform")
# endif
#else
    using std::signal;
#endif

} // namespace impl
} // namespace os
} // namespace yarp


#endif // YARP_OS_IMPL_PLATFORMSIGNAL_H
