/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_AWAITSYSTEMREADY_NWS_YARP_H

#include <yarp/dev/DeviceDriver.h>


/**
 * @ingroup dev_impl_network_servers
 *
 * \section AwaitSystemReady_nws_yarp Device description
 * \brief `AwaitSystemReady_nws_yarp`: A yarp nws to wait port for synchronization:
 *  it waits for the specified ports inside the PORT_LIST group.
 *  Related to SystemReady_nws_yarp.
 *
 * Parameters required by this device are:
 * | Parameter name      | SubParameter            | Type    | Units          | Default Value | Required                       | Description                                          |
 * |:-------------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:-----------------------------------------------------|
 * | PORT_NAME           |      -                  | group   |                |               | Yes                            | the group inside which port names are located        |
 *
 * example of xml file with a fake odometer
 *
 * \code{.unparsed}
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
 * <robot name="awaitSystemReady" build="2" portprefix="test" xmlns:xi="http://www.w3.org/2001/XInclude">
 * <devices>
 *   <device xmlns:xi="http://www.w3.org/2001/XInclude" name="awaitSystemReady_nws_yarp" type="awaitSystemReady_nws_yarp">
 *     <group name="PORT_LIST">
 *       <param name="pippo_port">/pippo</param>
 *       <param name="pluto_port">/pluto</param>
 *     </group>
 *   </device>
 * </devices>
 * </robot>
 * \endcode
 *
 */

class AwaitSystemReady_nws_yarp :
        public yarp::dev::DeviceDriver
{
public:
    AwaitSystemReady_nws_yarp();

    // DeviceDriver
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

};

#endif // YARP_AWAITSYSTEMREADY_NWS_YARP_H
