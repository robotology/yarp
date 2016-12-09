/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IDL_WIREVOCAB_H
#define YARP_OS_IDL_WIREVOCAB_H

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireVocab;
        }
    }
}

class YARP_OS_API yarp::os::idl::WireVocab {
public:
    virtual ~WireVocab() {}
    virtual int fromString(const std::string& input) = 0;
    virtual std::string toString(int input) = 0;
};

#endif // YARP_OS_IDL_WIREVOCAB_H
