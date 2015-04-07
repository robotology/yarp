/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

service robotInterfaceRpc {

    /**
     * Returns current phase.
     */
    string get_phase();

    /**
     * Returns current level.
     */
    i32 get_level();

    /**
     * Returns true if robotInterface is ready (all startup actions
     * performed and no interrupt called).
     */
    bool is_ready();

    /**
     * Returns robot name.
     */
    string get_robot();

    /**
     * Closes robotInterface.
     */
    string quit();

    /**
     * Closes robotInterface.
     */
    string bye();

    /**
     * Closes robotInterface.
     */
    string exit();
}
