/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Lorenzo Natale, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

import yarp.Network;

//run this to load the yarp library and initialize the network in Java

class LoadYarp{
	private static int yarpInitialized=0;

	public static int isLoaded()
	{ return yarpInitialized; }

    public LoadYarp() {
		if (yarpInitialized==0)
			{
				System.loadLibrary("jyarp");
				Network.init(); 
				System.out.println("Yarp library loaded and initialized");
				System.out.flush();
			}
		else
			{
				System.out.println("Yarp library already loaded and initialized, doing nothing");
				System.out.flush();
			}
			
		yarpInitialized=1;
    }
}
