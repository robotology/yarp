/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include <yarp/os/Network.h>
#include <mutex>
#include <yarp/os/Property.h>
#include <yarp/os/Thread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

class Callback:public BufferedPort<Bottle>
{
private:
    std::mutex mutex;
    Bottle Datum;

public:
    Callback()
    {
        Datum.clear();
        Datum.addString("null bottle");
    }

    void onRead(Bottle &v)
    {
        mutex.lock();
        Datum=v;
        //Time::delay(5);
        mutex.unlock();
        fprintf(stderr, "Callback got: %s\n",Datum.toString().c_str());
    }

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
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
