/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
