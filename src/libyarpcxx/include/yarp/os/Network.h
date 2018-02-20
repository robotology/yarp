/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCXX_Network_INC
#define YARPCXX_Network_INC

#include <yarp/yarpcxx.h>

/**
 *
 * C++ wrappers around an experimental C interface to YARP.
 *
 */
namespace yarpcxx {
    /**
     *
     * C++ wrappers around an experimental C interface to yarp::os.
     *
     */
    namespace os {
        class Network;
    }
}

class yarpcxx::os::Network {
public:
    Network() {
        impl = yarpNetworkCreate();
        YARPCXX_VALID(impl);
    }

    ~Network() {
        yarpNetworkFree(impl);
    }

    static bool setLocalMode(int isLocal) {
        return yarpNetworkSetLocalMode(NULL,isLocal) == 0;
    }

    static bool connect(const char *src, const char *dest, 
                        const char *carrier=NULL) {
        return yarpNetworkConnect(NULL,src,dest,carrier) == 0;
    }
private:
    yarpNetworkPtr impl;
};

#endif
