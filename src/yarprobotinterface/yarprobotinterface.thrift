/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

service yarprobotinterfaceRpc {

    /**
     * Returns current phase.
     */
    string get_phase();

    /**
     * Returns current level.
     */
    i32 get_level();

    /**
     * Returns robot name.
     */
    string get_robot();

    /**
     * Returns true if yarprobotinterface is ready (all startup actions
     * performed and no interrupt called).
     */
    bool is_ready();

    /**
     * Closes yarprobotinterface.
     */
    string quit();

    /**
     * Closes yarprobotinterface.
     */
    string bye();

    /**
     * Closes yarprobotinterface.
     */
    string exit();
}
