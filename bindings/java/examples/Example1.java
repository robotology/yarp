/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
	    bot.addDouble(10.4);
	    bot.addString("bozo");
	    System.out.println("sending bottle: " + bot);
	    p.write(bot);
	    Time.delay(1);
	}
	//Network.fini(); // never reached
    }
}
