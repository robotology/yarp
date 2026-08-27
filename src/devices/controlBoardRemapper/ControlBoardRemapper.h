/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H

#include <yarp/os/Network.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <string>
#include <vector>


#include "ControlBoardRemappingEngine.h"
#include "ControlBoardRemapper_ParamsParser.h"
#include "ControlBoardRemapperHelpers.h"

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/**
 * @ingroup dev_impl_remappers
 *
 * @brief `controlboardremapper` : device that takes a list of axes from multiple controlboards and expose them as a single controlboard.
 *
 * Parameters required by this device are shown in class: ControlBoardRemapper_ParamsParser
 */

class ControlBoardRemapper : public ControlBoardRemappingEngine,
                             public ControlBoardRemapper_ParamsParser,
                             public yarp::dev::DeviceDriver,
                             public yarp::dev::IMultipleWrapper

{
public:
    ControlBoardRemapper() = default;
    ControlBoardRemapper(const ControlBoardRemapper&) = delete;
    ControlBoardRemapper(ControlBoardRemapper&&) = delete;
    ControlBoardRemapper& operator=(const ControlBoardRemapper&) = delete;
    ControlBoardRemapper& operator=(ControlBoardRemapper&&) = delete;
    ~ControlBoardRemapper() override = default;

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    bool close() override;

    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are described in the class documentation.
    */
    bool open(yarp::os::Searchable &prop) override;

    bool detachAll() override;

    bool attachAll(const yarp::dev::PolyDriverList &l) override;
};

#endif // YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
