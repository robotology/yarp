/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREVOCAB
#define YARP2_WIREVOCAB

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

#endif
