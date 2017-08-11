/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_PORTREADERBUFFERBASECREATOR_H
#define YARP_OS_PORTREADERBUFFERBASECREATOR_H

#include <yarp/os/api.h>

namespace yarp { namespace os { class PortReaderBufferBaseCreator; }}

namespace yarp {
namespace os {

class YARP_OS_API PortReaderBufferBaseCreator
{
public:
    virtual ~PortReaderBufferBaseCreator();

    virtual yarp::os::PortReader *create() = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTREADERBUFFERBASECREATOR_H
