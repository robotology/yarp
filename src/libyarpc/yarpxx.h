// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 * This is a C++ wrapper of the plain C interface to YARP. 
 * It is as yet uncomplete.
 *
 */

#ifndef YET_ANOTHER_ROBOT_PLATFORM_CXXVERSION_INC
#define YET_ANOTHER_ROBOT_PLATFORM_CXXVERSION_INC

#include "yarp.h"

namespace Yarpxx {

    class Network {
    public:
        Network() {
            implementation = yarpNetworkCreate();
        }
        
        virtual ~Network() {
            yarpNetworkFree(network);
        }
        
        int setLocalMode(int isLocal) {
            return yarpNetworkSetLocalMode(implementation,isLocal);
        }

        int connect(const char *src, const char *dest, const char *carrier) {
            return yarpNetworkConnect(implementation,src,dest,carrier);
        }

        int disconnect(const char *src, const char *dest) {
            return yarpNetworkDisconnect(implementation,src,dest);
        }

    private:
        yarpNetworkPtr implementation;
    };




}

#endif

