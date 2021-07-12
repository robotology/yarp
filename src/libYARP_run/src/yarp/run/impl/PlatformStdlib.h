/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_PLATFORMSTDLIB_H
#define YARP_RUN_IMPL_PLATFORMSTDLIB_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/OS_NS_stdlib.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <stdlib.h>
#endif

namespace yarp {
namespace run {
namespace impl {


#if defined(YARP_HAS_ACE)
using std::getenv;
#    if defined(_MSC_VER)
// ACE bindings for setenv and unsetenv do not work
// on Visual Studio (last tested ACE 6.4.2, VS 2015).
inline int setenv(const char* name, const char* value, int overwrite)
{
    YARP_UNUSED(overwrite);
    return _putenv_s(name, value);
}
inline int unsetenv(const char* name)
{
    return _putenv_s(name, "");
}
#    else
using ACE_OS::setenv;
using ACE_OS::unsetenv;
#    endif
using ACE_OS::putenv;
#else
using std::getenv;
using ::setenv;
using ::unsetenv;
using ::putenv;
#endif

} // namespace impl
} // namespace run
} // namespace yarp

#endif // YARP_RUN_IMPL_PLATFORMSTDLIB_H
