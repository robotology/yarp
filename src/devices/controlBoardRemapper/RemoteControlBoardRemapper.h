/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDREMAPPER_REMOTECONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDREMAPPER_REMOTECONTROLBOARDREMAPPER_H

#include <yarp/dev/PolyDriver.h>

#include "ControlBoardRemapper.h"

/**
 *  @ingroup dev_impl_network_clients
 *
 * @brief `remotecontrolboardremapper` A device that takes a list of axes from
 * multiple controlboards, a list of remote controlboards in which this axes are
 * located, that is opening all the remote controlboards but is exposing them
 *
 * \section RemoteControlBoardRemapper
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | axesNames      |      -         | vector of strings  | -   |   -           | Yes          | Ordered list of the axes that are part of the remapped device.    |       |
 * | remoteControlBoards |     -     | vector of strings  | -   |   -           | Yes          | List of remote prefix used by the remote controlboards.           | The element of this list are then passed as "remote" parameter to the RemoteControlBoard device. |
 * | localPortPrefix |     -         | string             | -   |   -           | Yes          | All ports opened by this device will start with this prefix       |       |
 * | REMOTE_CONTROLBOARD_OPTIONS | - | group              | -   |   -           | No           | Options that will be passed directly to the remote_controlboard devices | |
 * All the passed remote controlboards are opened, and then the axesNames and the opened device are
 * passed to the ControlBoardRemapper device. If different axes
 * in two attached controlboard have the same name, the behaviour of this device is undefined.
 *
 *
 * Configuration file using .ini format.
 *
 * \code{.unparsed}
 *  device remotecontrolboardremapper
 *  axesNames (torso_pitch torso_roll torso_yaw neck_pitch neck_roll neck_yaw)
 *  remoteControlBoards (/icub/torso /icub/head)
 *
 *  [REMOTE_CONTROLBOARD_OPTIONS]
 *  writeStrict on
 *
 *
 * ...
 * \endcode
 *
 * Configuration of the device from C++ code.
 * \code{.cpp}
 *   Property options;
 *   options.put("device","remotecontrolboardremapper");
 *   Bottle axesNames;
 *   Bottle & axesList = axesNames.addList();
 *   axesList.addString("torso_pitch");
 *   axesList.addString("torso_roll");
 *   axesList.addString("torso_yaw");
 *   axesList.addString("neck_pitch");
 *   axesList.addString("neck_roll");
 *   axesList.addString("neck_yaw");
 *   options.put("axesNames",axesNames.get(0))
 *
 *   Bottle remoteControlBoards;
 *   Bottle & remoteControlBoardsList = remoteControlBoards.addList();
 *   remoteControlBoardsList.addString("/icub/torso");
 *   remoteControlBoardsList.addString("/icub/head");
 *   options.put("remoteControlBoards",remoteControlBoards.get(0));
 *
 *   options.put("localPortPrefix",/test");
 *
 *   Property & remoteControlBoardsOpts = options.addGroup("REMOTE_CONTROLBOARD_OPTIONS");
 *   remoteControlBoardsOpts.put("writeStrict","on");
 *
 *   // Actually open the device
 *   PolyDriver robotDevice(options);
 *
 *   // Use it as  you would use any controlboard device
 *   // ...
 * \endcode
 *
 *
 *
 * \section Caveat
 * By design, the RemoteControlBoardRemapper is more limited with respect to a true RemoteControlBoard.
 * Known limitations include:
 *   * If some axes belong to a coupled mechanics, all the axes should be added to the RemoteControlBoardRemapper.
 *     If only an axis of a coupled mechanics is added to the remapper, the semantic of the coupled mechanics
 *     in the underlyng implementation could create confusing behaviour. For example, changing the control mode
 *     of an axis in a coupled mechanism could change the control mode of the other coupled axes, even if the
 *     other coupled axes are not part of the remapped controlboard.
 *   * The debug methods provided by IRemoteVariables are not supported by the RemoteControlBoardRemapper .
 *
 */

class RemoteControlBoardRemapper : public ControlBoardRemapper
{
private:
    /**
     * List of remote_controlboard devices opened by the RemoteControlBoardRemapper device.
     */
    std::vector<yarp::dev::PolyDriver*> m_remoteControlBoardDevices;


    // Close all opened remote controlboards
    void closeAllRemoteControlBoards();

public:
    RemoteControlBoardRemapper() = default;
    RemoteControlBoardRemapper(const RemoteControlBoardRemapper&) = delete;
    RemoteControlBoardRemapper(RemoteControlBoardRemapper&&) = delete;
    RemoteControlBoardRemapper& operator=(const RemoteControlBoardRemapper&) = delete;
    RemoteControlBoardRemapper& operator=(RemoteControlBoardRemapper&&) = delete;
    ~RemoteControlBoardRemapper() override = default;

   /**
     * Open the device driver.
     * @param prop is a Searchable object which contains the parameters.
     * Allowed parameters are described in the class documentation.
     */
    bool open(yarp::os::Searchable &prop) override;

    /**
     * Close the device driver by deallocating all resources and closing ports.
     * @return true if successful or false otherwise.
     */
    bool close() override;
};

#endif
