// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>

#include <math.h>

using namespace yarp::os;

// Port latency, basic test.
// Send a sequence of message to a port. Test the 
// time it takes for the message to be received.
// Compute average.
//
// Time only makes sense if server and client run
// on the same machine.
// 
// Lorenzo Natale May 2008
//
// Added paralell port code August 2008.

// Parameters:
// --name: give a name to the port
// --server: act as a server
// --client: act as a client
// --period: if server set the periodicity of the messages [ms]
// --nframes: if client specifies how many message are received
// before closing (default: waits forever)

#include <ppEventDebugger.h>
#include <string>

#include <vector>

class Reader : public TypedReaderCallback<Bottle> {
public:
    BufferedPort<Bottle> outPort;

    double delay;
    double delaySq;
    int count;
    int wait;

    std::vector<double> latencies;

public:
    Reader()
    {
        delay=0;
        delaySq=0;
        count=0;
        // wait some messages before counting
        wait=20;
    }

    void onRead(Bottle& datum) {
        static ppEventDebugger pp;
        static bool init=false;

        double now=Time::now();
        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        pp.set();
        pp.reset();

        double t=datum.get(0).asDouble();
        if (wait<=0)
            {

                double dT=(now-t)*1000;
                delay+=dT;
                delaySq+=(dT*dT);
                count++;

                latencies.push_back(dT);
            }
        else
            wait--;

        Bottle& b = outPort.prepare();
        b.clear();
        b.addDouble(t);
        outPort.write();
    }
};

class serverThread: public RateThread
{
    BufferedPort<Bottle> port;

public:
    serverThread():RateThread(100)
    {}

    void open(double period, const std::string &name)
    {
        std::string portName;
        portName="/profiling/server/";
        portName+=name;
        portName+="/port:o";
        port.open(portName.c_str());

        RateThread::setRate(int (period*1000.0+0.5));
        RateThread::start();
    }

    void run()
    {
        static ppEventDebugger pp;
        static bool init=false;
        if (!init)
            {
                pp.open(0x378);
                init=true;
            }

        //  printf("Sending frame %d\n", k);
        Bottle& b = port.prepare();
        b.clear();
        double time=Time::now();
        b.addDouble(time);
        pp.set();
        port.write(true);
        pp.reset();
    }

    bool releaseThread()
    {
        port.close();
    }
}; 

int server(double server_wait, const std::string &name)
{
    serverThread thread;
    thread.open(server_wait, name);

    bool forever=true;
    while(forever)
        Time::delay(2);

    return 0;
}

int client(int nframes, std::string &name)
{
    Reader reader;
    std::string portName;
    portName="/profiling/client/";
    portName+=name;
    portName+="/port:i";

    BufferedPort<Bottle> port;
    port.useCallback(reader);
    port.open(portName.c_str());

    portName="/profiling/client/";
    portName+=name;
    portName+="/port:o";
    
    reader.outPort.open(portName.c_str());

    bool forever=false;
    if (nframes==-1)
        forever=true;
        
    while( (reader.count<nframes) || forever)
        {
            //give the CPU some time
            Time::delay(0.5);
        }

    port.close();

    std::vector<double>::iterator it=reader.latencies.begin();
    while(it!=reader.latencies.end())
        {
            printf("%lf\n", *it);
            it++;
        }

    double averageLatency=reader.delay/reader.count;
    double stdLatency=(1.0/(reader.count-1))*(reader.delaySq-reader.count*averageLatency*averageLatency);

    stdLatency=sqrt(stdLatency);

    fprintf(stderr, "Received: %d average latency %.3lf +/- %.5lf [ms]\n", 
            reader.count, averageLatency, stdLatency);
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
        return server(p.find("period").asDouble()/1000.0, name);
    else if (p.check("client"))
        return client(p.find("nframes").asInt(), name);
}






