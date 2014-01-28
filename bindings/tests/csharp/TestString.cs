using System;

// Copyright: (C) 2014 iCub Facility
// Author: Paul Fitzpatrick
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
// I don't really know C#, just bluffing from Java

namespace HelloNameSpace
{
    public class TestString
    {
	static void Main(string[] args)
	{
	    Network.init();
	    BufferedPortBottle p = new BufferedPortBottle();
	    p.open("/csharp");
	    p.close();
	    Network.fini();
	}
    }
}

