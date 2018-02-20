/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCXX_Port_INC
#define YARPCXX_Port_INC

#include <yarp/yarpcxx.h>

#include <yarp/os/Portable.h>

namespace yarpcxx {
    namespace os {
        class Port;
    }
}

class yarpcxx::os::Port {
public:
    Port() {
        impl = yarpPortCreate(NULL);
        YARPCXX_VALID(impl);
    }

    ~Port() {
        close();
        yarpPortFree(impl);
    }
   
    bool open(const char *name) {
      return yarpPortOpen(impl,name)==0;
    }

    bool close() {
        return yarpPortClose(impl)==0;
    }

    bool enableBackgroundWrite(int writeInBackgroundFlag) {
        return yarpPortEnableBackgroundWrite(impl,writeInBackgroundFlag)==0;
    }

    bool write(Portable& msg) {
        return yarpPortWrite(impl,msg.getHandle())==0;
    }

    bool read(Portable& msg) {
        return yarpPortRead(impl,msg.getHandle(),0)==0;
    }

private:
    yarpPortPtr impl;
};

#endif
