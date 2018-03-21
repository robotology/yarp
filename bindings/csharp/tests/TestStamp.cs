using System;

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// I don't really know C#, just bluffing from Java

namespace HelloNameSpace
{
    public class TestStamp
    {
	static void Main(string[] args)
	{
	    Network.init();
	    BufferedPortBottle p = new BufferedPortBottle();
	    if (!p.open("/csharp")) System.Environment.Exit(1);
	    Stamp ts = new Stamp(); 
	    p.setEnvelope(ts);
	    p.close();
	    Network.fini();
	}
    }
}

