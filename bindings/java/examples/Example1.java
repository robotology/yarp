/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

import yarp.Network;
import yarp.Bottle;
import yarp.Time;
import yarp.Port;

class Example1 {

    public static void main(String[] args) {
        System.loadLibrary("yarp_java");
        Network.init();
        Port p = new Port();
        p.open("/foo");
        // connect to a port called "/bar" if present
        Network.connect("/foo","/bar");
        while(true) {
            Bottle bot = new Bottle();
            bot.addFloat64(10.4);
            bot.addString("bozo");
            System.out.println("sending bottle: " + bot);
            p.write(bot);
            Time.delay(1);
        }
        //Network.fini(); // never reached
    }
}
