/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp testing

service ClockServer {
    oneway void pauseSimulation();
    oneway void continueSimulation();
    oneway void stepSimulation(1:i32 numberOfSteps = 1);
}
