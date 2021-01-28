/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIREVOCAB_H
#define YARP_OS_IDL_WIREVOCAB_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {
namespace idl {

class YARP_os_API WireVocab
{
public:
    virtual ~WireVocab() {}
    virtual int fromString(const std::string& input) = 0;
    virtual std::string toString(int input) const = 0;
};

} // namespace idl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IDL_WIREVOCAB_H
