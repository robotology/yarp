/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp testing

service ClockServer {
    oneway void pauseSimulation();
    oneway void continueSimulation();
    oneway void stepSimulation(1:i32 numberOfSteps = 1);
}
