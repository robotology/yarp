/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_WIREVOCAB_H
#define YARP_OS_IDL_WIREVOCAB_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_ICONTROLLIMITS2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/idl/WireVocab.h> file is deprecated")
#endif


#include <yarp/os/api.h>

#include <string>

namespace yarp::os::idl {

class YARP_os_DEPRECATED_API WireVocab
{
public:
    virtual ~WireVocab() {}
    virtual int fromString(const std::string& input) = 0;
    virtual std::string toString(int input) const = 0;
};

} // namespace yarp::os::idl

#endif // YARP_OS_IDL_WIREVOCAB_H
