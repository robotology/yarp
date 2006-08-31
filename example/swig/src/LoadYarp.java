//run this to load the yarp library and initialize the network in Java

class LoadYarp{
    public LoadYarp() {
	System.loadLibrary("jyarp");
	Network.init(); 
    }
}
