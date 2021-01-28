/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

package yarp.matlab;

import yarp.Network;

// run this to load the yarp library and initialize the network in Java

class LoadYarp {
    private static int yarpInitialized = 0;

    public static int isLoaded() { return yarpInitialized; }

    public LoadYarp() {
        if (yarpInitialized == 0) {
            System.loadLibrary("yarp_java");
            Network.init();
            System.out.println("Yarp library loaded and initialized");
            System.out.flush();
        } else {
            System.out.println(
                "Yarp library already loaded and initialized, doing nothing");
            System.out.flush();
        }
        yarpInitialized = 1;
    }
}
