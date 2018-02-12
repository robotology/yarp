/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>

#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Property.h>
#include <yarp/os/Thread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

class Callback:public BufferedPort<Bottle>
{
private:
	Semaphore mutex;
	Bottle Datum;

public:
	Callback()
	{
        Datum.clear();
        Datum.addString("null bottle");
	}

  	void onRead(Bottle &v)
	{
        mutex.wait(); 
        Datum=v;
        //Time::delay(5);
        mutex.post();
        fprintf(stderr, "Callback got: %s\n",Datum.toString().c_str());
    }

	void lock()
	{
		mutex.wait();
	}

    void unlock()
    {
        mutex.post();
    }

    Bottle get()
    {
        return Datum;
    }
};

int main(int argc, char **argv)
{
    Network yarp;

    Property parameters;
    parameters.fromCommand(argc, argv);

    Callback cback;

    cback.open("/callback");
    cback.useCallback();

    bool done=false;
    while(!done)
    {
        Time::delay(1);
        cback.lock();
        Bottle b=cback.get();
        cback.unlock();
        fprintf(stderr, "Main got: %s\n", b.toString().c_str());
        if (b.get(0).asString()=="quit")
            done=true;
    }
    
    
    fprintf(stderr, "Closing the port...\n");
    cback.close();
    fprintf(stderr, "done\n");
    fprintf(stderr, "Now returning from main\n");
    return 0;
}
