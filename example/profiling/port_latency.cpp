// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

// Port latency, basic test.
// Send a sequence of message to a port. Test the 
// time it takes for the message to be received.
// Compute average.
//
// Time only makes sense if server and client are run
// on the same machine.
// 
// Lorenzo Natale May 2008
//
// Added paralell port code August 2008.

#include <ppEventDebugger.h>
#include <string>

class Reader : public TypedReaderCallback<Bottle> {
public:
    double delay;
    int count;
    int wait;
public:
    Reader()
    {
        delay=0;
        count=0;
        // wait some messages before counting
        wait=10;
    }

    void onRead(Bottle& datum) {

        static ppEventDebugger pp;
        static bool init=false;
        
        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        int pulseWidth=100;
        while(pulseWidth--)
            pp.set();
        pp.reset();

        //        if (wait>0)
        //            {
        //                wait--;
        //            }
        //        else
        {

                double t=datum.get(0).asDouble();
                double now=Time::now();
                delay+=(now-t)*1000;
                count++;
                // remove this to spare cpu time
                //                fprintf(stderr, "%lf\n", (now-t)*1000);
            }
        //        else

    }
};

int server(double server_wait, const std::string &name)
{
    BufferedPort<Bottle> port;
    std::string portName;
    portName="/profiling/server/";
    portName+=name;
    portName+="/port";
    port.open(portName.c_str());

    int k=0;
    const int batchSize=5;

    while(true) {

        static ppEventDebugger pp;
        static bool init=false;
        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        int size=batchSize;
        while(size--)
            {
                printf("Sending frame %d\n", k);
                Bottle& b = port.prepare();
                b.clear();
                double time=Time::now();
                b.addDouble(time);

                int pulseWidth=100;
                while(pulseWidth--)
                    pp.set();
                port.write(true);
                pp.reset();
                //give the CPU some time
                Time::delay(server_wait);
                k++;
            }
        //        pp.set();
        //        Time::delay(server_wait*3);
        //        pp.reset();
        //        Time::delay(server_wait);
    }
    port.close();
    return 0;
}

int client(int nframes, std::string &name)
{
    Reader reader;
    std::string portName;
    portName="/profiling/client/";
    portName+=name;
    portName+="/port";

    BufferedPort<Bottle> port;
    port.useCallback(reader);
    port.open(portName.c_str());

    bool forever=false;
    if (nframes=-1)
        forever=true;
        
    while( (reader.count<nframes) || forever)
        {
            //give the CPU some time
            Time::delay(0.5);
        }

    port.close();

    fprintf(stderr, "Received: %d average latency %.3lf[ms]\n", 
            reader.count, reader.delay/reader.count);
    return 0;
}

int main(int argc, char **argv) {
    Network yarp;
    Property p;
    p.fromCommand(argc, argv);

    std::string name;
    if (p.check("name"))
        name=p.find("name").asString().c_str();
    else
        name="default";

    if (p.check("server"))
        return server(p.find("period").asDouble(), name);
    else if (p.check("client"))
        return client(p.find("nframes").asInt(), name);
}
