/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
