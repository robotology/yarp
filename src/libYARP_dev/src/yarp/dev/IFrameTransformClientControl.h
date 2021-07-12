/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMETRANSFORM_CLIENTCONTROL_H
#define YARP_DEV_IFRAMETRANSFORM_CLIENTCONTROL_H

#include <string>
#include <vector>
#include <yarp/dev/api.h>
#include <yarp/os/Vocab.h>

namespace yarp {
    namespace dev {
        class IFrameTransformClientControl;
      }
}

/**
 * @ingroup dev_iface_transform
 *
 * IFrameTransformClientControl Interface.
 */
class YARP_dev_API yarp::dev::IFrameTransformClientControl
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameTransformClientControl();

    /**
    * Returns true if the client is connected with the server, false otherwise.
    * @return true/false
    */
    virtual bool isConnectedWithServer() = 0;

    /**
    * Attempts to reconnect the client with the server. Returns true if the operation
    * is succesful, false otherwise.
    * @return true/false
    */
    virtual bool reconnectWithServer() = 0;
};


#endif // YARP_DEV_IFRAMETRANSFORM_CLIENTCONTROL_H
