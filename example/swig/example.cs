using System;

// I don't really know C#, just bluffing from Java

namespace HelloNameSpace
{
    public class HelloWorld
    {
	static void Main(string[] args)
	{
	    Network.init();
	    BufferedPortBottle p = new BufferedPortBottle();
	    p.open("/csharp");
	    int top = 100;
	    for (int i=1; i<top; i++) {
		Bottle bottle = p.prepare();
		bottle.clear();
		bottle.addString("count");
		bottle.addInt(i);
		bottle.addString("of");
		bottle.addInt(top);
		Console.WriteLine("Sending " + bottle.toString());
		p.write();
		Time.delay(0.5);
	    }
	    p.close();
	    Network.fini();
	}
    }
}

