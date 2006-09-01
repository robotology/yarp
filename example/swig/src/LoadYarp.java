//run this to load the yarp library and initialize the network in Java

class LoadYarp{
	static int yarpInitialized=0;
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
