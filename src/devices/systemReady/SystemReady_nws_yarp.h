/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SYSTEMREADY_NWS_YARP_H
#define YARP_SYSTEMREADY_NWS_YARP_H

#include <yarp/sig/Vector.h>
#include <yarp/dev/DeviceDriver.h>


/**
 * @ingroup dev_impl_network_clients dev_impl_navigation
 *
 * \section SystemReady_nws_yarp_parameters Device description
 * \brief `SystemReady_nws_yarp`: A yarp nws to open port for synchronization:
 *  it opens the specified ports inside the PORT_LIST group.
 *  Related to awaitSystemReady_nws_yarp.
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
 * <robot name="fakeOdometry" build="2" portprefix="test" xmlns:xi="http://www.w3.org/2001/XInclude">
 * <devices>
 *   <device xmlns:xi="http://www.w3.org/2001/XInclude" name="systemReady_nws_yarp" type="systemReady_nws_yarp">
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

class SystemReady_nws_yarp :
        public yarp::dev::DeviceDriver
{
public:
    SystemReady_nws_yarp();

    // DeviceDriver
    bool open(yarp::os::Searchable &params) override;
    bool close() override;

private:
    std::vector<yarp::os::Port*> port_pointers_list;

};

#endif // YARP_SYSTEMREADY_NWS_YARP_H
