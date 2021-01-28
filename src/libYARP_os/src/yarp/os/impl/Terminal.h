/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_TERMINAL_H
#define YARP_OS_IMPL_TERMINAL_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {
namespace impl {
namespace Terminal {

YARP_os_impl_API
bool EOFreached();

YARP_os_impl_API
std::string getStdin();

YARP_os_impl_API
std::string readString(bool* eof);

} // namespace Terminal
} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TERMINAL_H
